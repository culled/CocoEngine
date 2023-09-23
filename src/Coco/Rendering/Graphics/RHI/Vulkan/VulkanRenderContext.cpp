#include "Renderpch.h"
#include "VulkanRenderContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "CachedResources/VulkanFramebuffer.h"
#include "CachedResources/VulkanRenderPass.h"
#include "VulkanBuffer.h"

#include <Coco/Core/Math/Random.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanContextRenderOperation::VulkanContextRenderOperation(
		VulkanFramebuffer& framebuffer, 
		VulkanRenderPass& renderPass) :
		Framebuffer(framebuffer),
		RenderPass(renderPass),
		ViewportRect{},
		ScissorRect{},
		StateChanges{},
		CurrentShaderID{},
		CurrentInstanceID{},
		BoundPipeline{},
		BoundGlobalDescriptors{},
		BoundInstanceDescriptors{}
	{}

	VulkanRenderContext::VulkanRenderContext(const GraphicsDeviceResourceID& id) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_vulkanRenderOperation{},
		_renderStartSemaphore(),
		_renderCompletedSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
		_renderCompletedFence(CreateManagedRef<VulkanGraphicsFence>(id, false)),
		_waitOnSemaphores{},
		_renderCompletedSignalSemaphores{},
		_commandBuffer{},
		_deviceCache(_device.GetCache())
	{
		DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
		{
			throw std::exception("A graphics queue is required for rendering operations");
		}

		_commandBuffer = graphicsQueue->Pool.Allocate(true);
		_renderCompletedSignalSemaphores.push_back(_renderCompletedSemaphore);
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		_renderStartSemaphore.Invalidate();
		_renderCompletedSemaphore.Invalidate();
		_renderCompletedFence.Invalidate();

		DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (graphicsQueue)
		{
			graphicsQueue->Pool.Free(*_commandBuffer);
		}

		_commandBuffer.reset();

		_device.PurgeUnusedResources();
	}

	void VulkanRenderContext::WaitForRenderingToComplete()
	{
		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
	}

	Ref<GraphicsSemaphore> VulkanRenderContext::GetOrCreateRenderStartSemaphore()
	{
		if (!_renderStartSemaphore.IsValid())
		{
			_renderStartSemaphore = CreateManagedRef<VulkanGraphicsSemaphore>(ID);
			_waitOnSemaphores.push_back(_renderStartSemaphore);
		}

		return _renderStartSemaphore;
	}

	void VulkanRenderContext::AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Wait on semaphore was invalid")
				return;
		}

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_waitOnSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Signal semaphore was invalid")
				return;
		}

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_renderCompletedSignalSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::SetViewportRect(const RectInt& viewportRect)
	{
		Assert(_vulkanRenderOperation.has_value())
		_vulkanRenderOperation->ViewportRect = viewportRect;

		VkViewport viewport{};
		viewport.x = static_cast<float>(viewportRect.Offset.X);
		viewport.y = static_cast<float>(viewportRect.Offset.Y);
		viewport.width = static_cast<float>(viewportRect.Size.Width);
		viewport.height = static_cast<float>(viewportRect.Size.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);
	}

	void VulkanRenderContext::SetScissorRect(const RectInt& scissorRect)
	{
		Assert(_vulkanRenderOperation.has_value())
		_vulkanRenderOperation->ScissorRect = scissorRect;

		VkRect2D scissor{};
		scissor.offset.x = static_cast<uint32_t>(scissorRect.Offset.X);
		scissor.offset.y = static_cast<uint32_t>(scissorRect.Offset.Y);
		scissor.extent.width = static_cast<uint32_t>(scissorRect.Size.Width);
		scissor.extent.height = static_cast<uint32_t>(scissorRect.Size.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void VulkanRenderContext::SetMaterial(const MaterialData& material)
	{
		Assert(_vulkanRenderOperation.has_value())

		if (_vulkanRenderOperation->CurrentInstanceID.has_value() && _vulkanRenderOperation->CurrentInstanceID.value() == material.ID)
			return;

		const ShaderData& shader = _renderOperation->RenderView.GetShaderData(material.ShaderID);
		uint64 passKey = shader.RenderPassShaders.at(_renderOperation->CurrentPassName);
		const RenderPassShaderData& passShader = _renderOperation->RenderView.GetRenderPassShaderData(passKey);

		SetShader(passShader);

		_renderOperation->InstanceUniforms = material.UniformData;
		_vulkanRenderOperation->CurrentInstanceID = material.ID;

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Instance);
	}

	void VulkanRenderContext::SetShader(const RenderPassShaderData& shader)
	{
		Assert(_vulkanRenderOperation.has_value())

		if (_vulkanRenderOperation->CurrentShaderID.has_value() && _vulkanRenderOperation->CurrentShaderID.value() == shader.ID)
			return;

		_vulkanRenderOperation->CurrentShaderID = shader.ID;

		// Clear shader-specific uniforms
		_renderOperation->InstanceUniforms.Clear();
		_renderOperation->DrawUniforms.Clear();

		// Reset shader-specific bindings
		_vulkanRenderOperation->BoundPipeline.reset();
		_vulkanRenderOperation->BoundGlobalDescriptors.reset();
		_vulkanRenderOperation->BoundInstanceDescriptors.reset();

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Shader);
	}

	void VulkanRenderContext::Draw(const MeshData& mesh, uint32 submeshID)
	{
		Assert(mesh.Submeshes.contains(submeshID))

		const SubmeshData& submesh = mesh.Submeshes.at(submeshID);
		DrawIndexed(mesh, static_cast<uint32>(submesh.FirstIndexOffset), static_cast<uint32>(submesh.IndexCount));
	}

	void VulkanRenderContext::DrawIndexed(const MeshData& mesh, uint32 firstIndexOffset, uint32 indexCount)
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(mesh.IndexBuffer.IsValid())
		Assert(mesh.VertexBuffer.IsValid())

		if (!FlushStateChanges())
		{
			CocoError("Failed to setup state for mesh. Skipping...")
				return;
		}

		VkCommandBuffer cmdBuffer = _commandBuffer->GetCmdBuffer();

		// Bind the vertex buffer
		std::array<VkDeviceSize, 1> offsets = { 0 };
		VulkanBuffer* vertexBuffer = static_cast<VulkanBuffer*>(mesh.VertexBuffer.Get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		VulkanBuffer* indexBuffer = static_cast<VulkanBuffer*>(mesh.IndexBuffer.Get());
		vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Draw the mesh
		vkCmdDrawIndexed(cmdBuffer,
			indexCount,
			1,
			firstIndexOffset,
			0, 0);

		RenderContextRenderStats& stats = _renderOperation->Stats;
		stats.VertexCount += mesh.VertexCount;
		stats.TrianglesDrawn += indexCount / 3;
		_renderOperation->DrawUniforms.Clear();
	}

	bool VulkanRenderContext::BeginImpl()
	{
		try
		{
			// Get the images from the render targets
			std::span<const RenderTarget> rts = _renderOperation->RenderView.GetRenderTargets();
			std::vector<const VulkanImage*> vulkanImages(rts.size());

			for (size_t i = 0; i < rts.size(); i++)
			{
				Assert(rts[i].Image.IsValid())
				vulkanImages.at(i) = static_cast<const VulkanImage*>(rts[i].Image.Get());
			}

			VulkanRenderPass& renderPass = _deviceCache->GetOrCreateRenderPass(_renderOperation->Pipeline);
			VulkanRenderContextCache& cache = _deviceCache->GetOrCreateContextCache(ID);
			VulkanFramebuffer& framebuffer = cache.GetOrCreateFramebuffer(_renderOperation->RenderView.GetViewportRect().Size, renderPass, vulkanImages);

			// Setup the Vulkan-specific render operation
			_vulkanRenderOperation.emplace(VulkanContextRenderOperation(framebuffer, renderPass));

			const RectInt& viewportRect = _renderOperation->RenderView.GetViewportRect();
			_vulkanRenderOperation->ViewportRect = viewportRect;
			_vulkanRenderOperation->ScissorRect = _renderOperation->RenderView.GetScissorRect();

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			// Start the first render pass
			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = renderPass.GetRenderPass();
			beginInfo.framebuffer = framebuffer.GetFramebuffer();

			beginInfo.renderArea.offset.x = static_cast<uint32_t>(viewportRect.Offset.X);
			beginInfo.renderArea.offset.y = static_cast<uint32_t>(viewportRect.Offset.Y);
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(viewportRect.Size.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(viewportRect.Size.Height);

			std::vector<VkClearValue> clearValues(rts.size());

			// Set clear clear color for each render target
			for (size_t i = 0; i < clearValues.size(); i++)
			{
				const RenderTarget& rt = rts[i];
				const AttachmentFormat& attachment = _renderOperation->Pipeline.InputAttachments.at(i);
				VkClearValue& clearValue = clearValues.at(i);

				if (IsDepthFormat(attachment.PixelFormat) || IsStencilFormat(attachment.PixelFormat))
				{
					clearValue.depthStencil.depth = static_cast<float>(rt.ClearValue.X);
					clearValue.depthStencil.stencil = static_cast<uint32>(Math::Round(rt.ClearValue.Y));
				}
				else
				{
					clearValues[i].color =
					{
						static_cast<float>(rt.ClearValue.X),
						static_cast<float>(rt.ClearValue.Y),
						static_cast<float>(rt.ClearValue.Z),
						static_cast<float>(rt.ClearValue.W)
					};
				}
			}

			beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			beginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

			// Since viewport and scissors are dynamic, we must set them at least once
			SetViewportRect(_vulkanRenderOperation->ViewportRect);
			SetScissorRect(_vulkanRenderOperation->ScissorRect);
		}
		catch (const std::exception& ex)
		{
			CocoError("Could not begin VulkanRenderContext: {}", ex.what())
			return false;
		}

		return true;
	}

	bool VulkanRenderContext::BeginNextPassImpl()
	{
		Assert(_vulkanRenderOperation.has_value())
		vkCmdNextSubpass(_commandBuffer->GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		return true;
	}

	void VulkanRenderContext::EndImpl()
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(_currentState == State::InRender)

		vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());

		AddPostRenderPassImageTransitions();

		_commandBuffer->EndAndSubmit(
			&_waitOnSemaphores,
			&_renderCompletedSignalSemaphores,
			_renderCompletedFence);
	}

	void VulkanRenderContext::UniformChanged(UniformScope scope, ShaderUniformData::UniformKey key)
	{
		Assert(_vulkanRenderOperation.has_value())

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Uniform);
	}

	void VulkanRenderContext::AddPreRenderPassImageTransitions()
	{
		Assert(_vulkanRenderOperation.has_value())

		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);

			// Don't bother transitioning layouts for attachments that are cleared as Vulkan will handle that for us
			if (attachment.ShouldClear)
				continue;

			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			VulkanImage* image = static_cast<VulkanImage*>(_renderOperation->RenderView.GetRenderTarget(i).Image.Get());

			image->TransitionLayout(*_commandBuffer, layout);
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions()
	{
		Assert(_vulkanRenderOperation.has_value())

		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			VulkanImage* image = static_cast<VulkanImage*>(_renderOperation->RenderView.GetRenderTarget(i).Image.Get());

			// Since Vulkan automatically transitions layouts between passes, update the image's layout to match the layouts of the attachments
			image->_currentLayout = layout;

			// Transition any attachments that will be presented
			if ((image->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
			{
				image->TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
		}
	}

	bool VulkanRenderContext::FlushStateChanges()
	{
		Assert(_vulkanRenderOperation.has_value())

			// Early out if no state changes since last draw
			if (_vulkanRenderOperation->StateChanges.size() == 0)
				return true;

		bool stateBound = true;

		try
		{
			if (!_vulkanRenderOperation->CurrentShaderID.has_value())
				throw std::exception("No shader was bound");

			if (_vulkanRenderOperation->CurrentShaderID.value() == RenderView::InvalidID)
				throw std::exception("Invalid shader");

			const RenderPassShaderData& boundShaderData = _renderOperation->RenderView.GetRenderPassShaderData(_vulkanRenderOperation->CurrentShaderID.value());
			VulkanRenderPassShader& shader = _deviceCache->GetOrCreateShader(boundShaderData.ShaderData);

			VulkanRenderContextCache& cache = _deviceCache->GetOrCreateContextCache(ID);

			VulkanUniformData& uniformData = cache.GetOrCreateUniformData(shader);
			VulkanPipeline* pipeline = nullptr;
			VkCommandBuffer cmdBuffer = _commandBuffer->GetCmdBuffer();

			// Bind the new shader if it changed
			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Shader) ||
				!_vulkanRenderOperation->BoundGlobalDescriptors.has_value())
			{
				pipeline = &_device.GetCache()->GetOrCreatePipeline(
					_vulkanRenderOperation->RenderPass,
					_renderOperation->CurrentPassIndex,
					shader
				);

				_vulkanRenderOperation->BoundPipeline = pipeline;

				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

				VkDescriptorSet set = uniformData.PrepareData(VulkanUniformData::sGlobalInstanceID, _renderOperation->GlobalUniforms, shader, true);
				if (set)
				{
					// Bind the global descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline->GetPipelineLayout(),
						0, 1, &set,
						0, 0);

					_vulkanRenderOperation->BoundGlobalDescriptors = set;
				}
				else
				{
					_vulkanRenderOperation->BoundGlobalDescriptors.reset();
				}
			}

			Assert(_vulkanRenderOperation->BoundPipeline.has_value())
			pipeline = _vulkanRenderOperation->BoundPipeline.value();

			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Uniform) ||
				_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Instance) ||
				!_vulkanRenderOperation->BoundInstanceDescriptors.has_value())
			{
				uint64 instanceID;

				if (_vulkanRenderOperation->CurrentInstanceID.has_value())
				{
					instanceID = _vulkanRenderOperation->CurrentInstanceID.value();
				}
				else
				{
					instanceID = Random::Global.RandomRange(0, Math::MaxValue<int>());
				}

				VkDescriptorSet set = uniformData.PrepareData(instanceID, _renderOperation->InstanceUniforms, shader, true);

				if (set)
				{
					VulkanPipeline* pipeline = _vulkanRenderOperation->BoundPipeline.value();

					uint32 offset = _vulkanRenderOperation->BoundGlobalDescriptors.has_value() ? 1 : 0;

					// Bind the instance descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline->GetPipelineLayout(),
						offset, 1, &set,
						0, 0);

					_vulkanRenderOperation->BoundInstanceDescriptors = set;
				}
				else
				{
					_vulkanRenderOperation->BoundInstanceDescriptors.reset();
				}
			}

			uniformData.PreparePushConstants(*_commandBuffer, pipeline->GetPipelineLayout(), _renderOperation->DrawUniforms, shader);
		}
		catch (const std::exception& ex)
		{
			CocoError("Error flushing state changes: {}", ex.what())
			stateBound = false;
		}

		_vulkanRenderOperation->StateChanges.clear();
		return stateBound;
	}
}