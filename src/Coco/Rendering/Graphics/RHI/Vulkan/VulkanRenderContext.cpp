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
	BoundGlobalState::BoundGlobalState(uint64 shaderID) :
		ShaderID(shaderID),
		Pipeline(nullptr),
		DescriptorSet(nullptr)
	{}

	BoundInstanceState::BoundInstanceState(uint64 instanceID) :
		InstanceID(instanceID),
		DescriptorSet(nullptr)
	{}

	VulkanContextRenderOperation::VulkanContextRenderOperation(
		VulkanFramebuffer& framebuffer, 
		VulkanRenderPass& renderPass) :
		Framebuffer(framebuffer),
		RenderPass(renderPass),
		ViewportRect{},
		ScissorRect{},
		StateChanges{},
		GlobalState{},
		InstanceState{}
	{}

	VulkanRenderContext::VulkanRenderContext(const GraphicsDeviceResourceID& id) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_vulkanRenderOperation{},
		_renderStartSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
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
		if (_currentState == State::RenderCompleted)
			return;

		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
		_currentState = State::RenderCompleted;
	}

	bool VulkanRenderContext::CheckForRenderingComplete()
	{
		if (_currentState == State::EndedRender)
		{
			if (_renderCompletedFence->IsSignaled())
				_currentState = State::RenderCompleted;
		}

		return _currentState == State::RenderCompleted;
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

		if (_vulkanRenderOperation->InstanceState.has_value() && _vulkanRenderOperation->InstanceState->InstanceID == material.ID)
			return;

		const ShaderData& shader = _renderOperation->RenderView.GetShaderData(material.ShaderID);
		uint64 passKey = shader.RenderPassShaders.at(_renderOperation->CurrentPassName);
		const RenderPassShaderData& passShader = _renderOperation->RenderView.GetRenderPassShaderData(passKey);

		SetShader(passShader);

		_renderOperation->InstanceUniforms = material.UniformData;

		_vulkanRenderOperation->InstanceState.emplace(material.ID);
		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Instance);
	}

	void VulkanRenderContext::SetShader(const RenderPassShaderData& shader)
	{
		Assert(_vulkanRenderOperation.has_value())

		if (_vulkanRenderOperation->GlobalState.has_value() && _vulkanRenderOperation->GlobalState->ShaderID == shader.ID)
			return;

		_vulkanRenderOperation->GlobalState.emplace(shader.ID);

		// Clear shader-specific uniforms
		_renderOperation->InstanceUniforms.Clear();
		_renderOperation->DrawUniforms.Clear();

		// Reset shader-specific bindings
		_vulkanRenderOperation->InstanceState.reset();

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Shader);
	}

	void VulkanRenderContext::DrawIndexed(const MeshData& mesh, uint64 firstIndexOffset, uint64 indexCount)
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
			static_cast<uint32>(indexCount),
			1,
			static_cast<uint32>(firstIndexOffset),
			0, 0);

		_stats.VertexCount += mesh.VertexCount;
		_stats.TrianglesDrawn += indexCount / 3;
		_stats.DrawCalls++;

		_renderOperation->DrawUniforms.Clear();
	}

	bool VulkanRenderContext::BeginImpl()
	{
		try
		{
			_renderCompletedSignalSemaphores.push_back(_renderCompletedSemaphore);

			const RenderView& renderView = _renderOperation->RenderView;

			// Get the images from the render targets
			std::span<const RenderTarget> rts = renderView.GetRenderTargets();
			std::vector<const VulkanImage*> vulkanImages(rts.size());
			std::vector<uint8> resolveImageIndices;

			// Add primary attachments
			for (uint8 i = 0; i < rts.size(); i++)
			{
				Assert(rts[i].MainImage.IsValid())
				vulkanImages.at(i) = static_cast<const VulkanImage*>(rts[i].MainImage.Get());

				if (rts[i].ResolveImage.IsValid())
				{
					resolveImageIndices.push_back(i);
				}
			}
			
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

			// Add any resolve attachments
			for (const uint8& i : resolveImageIndices)
			{
				vulkanImages.push_back(static_cast<const VulkanImage*>(rts[i].ResolveImage.Get()));
				clearValues.push_back(clearValues.at(i));
			}

 			const RectInt& viewportRect = renderView.GetViewportRect();

			VulkanRenderPass& renderPass = _deviceCache.GetOrCreateRenderPass(_renderOperation->Pipeline, renderView.GetMSAASamples(), resolveImageIndices);
			VulkanRenderContextCache& cache = _deviceCache.GetOrCreateContextCache(ID);
			VulkanFramebuffer& framebuffer = cache.GetOrCreateFramebuffer(viewportRect.Size, renderPass, vulkanImages);

			// Setup the Vulkan-specific render operation
			_vulkanRenderOperation.emplace(VulkanContextRenderOperation(framebuffer, renderPass));

			_vulkanRenderOperation->ViewportRect = viewportRect;
			_vulkanRenderOperation->ScissorRect = renderView.GetScissorRect();

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

		_vulkanRenderOperation->GlobalState.reset();
		_vulkanRenderOperation->InstanceState.reset();

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

	void VulkanRenderContext::ResetImpl()
	{
		WaitForRenderingToComplete();

		_vulkanRenderOperation.reset();

		_waitOnSemaphores.clear();
		_renderCompletedSignalSemaphores.clear();
		_renderCompletedFence->Reset();
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
		std::span<RenderTarget> rts = _renderOperation->RenderView.GetRenderTargets();

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);

			// Don't bother transitioning layouts for attachments that are cleared as Vulkan will handle that for us
			if (attachment.ShouldClear)
				continue;

			RenderTarget& rt = rts[i];
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);

			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());
			image->TransitionLayout(*_commandBuffer, layout);

			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				image->TransitionLayout(*_commandBuffer, layout);
			}
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions()
	{
		Assert(_vulkanRenderOperation.has_value())

		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;
		std::span<RenderTarget> rts = _renderOperation->RenderView.GetRenderTargets();

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			RenderTarget& rt = rts[i];

			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());

			// Since Vulkan automatically transitions layouts between passes, update the image's layout to match the layouts of the attachments
			image->_currentLayout = layout;

			// Transition any attachments that will be presented
			if ((image->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
			{
				image->TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}

			// Do the same for the resolve image, if one exists
			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				image->_currentLayout = layout;

				if ((image->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
				{
					image->TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				}
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
			if (!_vulkanRenderOperation->GlobalState.has_value())
				throw std::exception("No shader was bound");

			BoundGlobalState& globalState = _vulkanRenderOperation->GlobalState.value();

			if (globalState.ShaderID == RenderView::InvalidID)
				throw std::exception("Invalid shader");

			const RenderPassShaderData& boundShaderData = _renderOperation->RenderView.GetRenderPassShaderData(globalState.ShaderID);
			VulkanRenderPassShader& shader = _deviceCache.GetOrCreateShader(boundShaderData.ShaderData);

			VulkanRenderContextCache& cache = _deviceCache.GetOrCreateContextCache(ID);

			VulkanUniformData& uniformData = cache.GetOrCreateUniformData(shader);
			VkCommandBuffer cmdBuffer = _commandBuffer->GetCmdBuffer();

			// Bind the new shader if it changed
			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Shader))
			{
				globalState.Pipeline = &_deviceCache.GetOrCreatePipeline(
					_vulkanRenderOperation->RenderPass,
					_renderOperation->CurrentPassIndex,
					shader
				);

				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, globalState.Pipeline->GetPipeline());

				globalState.DescriptorSet = uniformData.PrepareData(VulkanUniformData::sGlobalInstanceID, _renderOperation->GlobalUniforms, shader, true);
				if (globalState.DescriptorSet)
				{
					// Bind the global descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						globalState.Pipeline->GetPipelineLayout(),
						0, 
						1, &globalState.DescriptorSet,
						0, 0);
				}
			}

			Assert(globalState.Pipeline != nullptr)

			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Uniform) ||
				_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Instance))
			{
				if (!_vulkanRenderOperation->InstanceState.has_value())
				{
					_vulkanRenderOperation->InstanceState.emplace(static_cast<uint64>(Random::Global.RandomRange(0, Math::MaxValue<int>())));
				}

				BoundInstanceState& instanceState = _vulkanRenderOperation->InstanceState.value();

				instanceState.DescriptorSet = uniformData.PrepareData(instanceState.InstanceID, _renderOperation->InstanceUniforms, shader, true);

				if (instanceState.DescriptorSet)
				{
					uint32 offset = globalState.DescriptorSet ? 1 : 0;

					// Bind the instance descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						globalState.Pipeline->GetPipelineLayout(),
						offset, 
						1, &instanceState.DescriptorSet,
						0, 0);
				}
			}

			uniformData.PreparePushConstants(*_commandBuffer, globalState.Pipeline->GetPipelineLayout(), _renderOperation->DrawUniforms, shader);
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