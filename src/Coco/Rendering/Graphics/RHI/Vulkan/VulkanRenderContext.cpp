#include "Renderpch.h"
#include "VulkanRenderContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "CachedResources/VulkanFramebuffer.h"
#include "CachedResources/VulkanRenderPass.h"
#include "VulkanBuffer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanContextRenderOperation::VulkanContextRenderOperation(VulkanFramebuffer& framebuffer, VulkanRenderPass& renderPass) :
		Framebuffer(framebuffer),
		RenderPass(renderPass),
		WaitOnSemaphores{},
		RenderCompletedSignalSemaphores{},
		ViewportRect{},
		ScissorRect{},
		StateChanges{},
		CurrentShaderID{},
		BoundPipeline{}
	{}

	const uint32 VulkanRenderContext::sGlobalDescriptorSetIndex = 0;
	const uint32 VulkanRenderContext::sInstanceDescriptorSetIndex = 1;

	VulkanRenderContext::VulkanRenderContext(const GraphicsDeviceResourceID& id) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_backbufferIndex(-1),
		_vulkanRenderOperation{},
		_imageAvailableSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
		_renderCompletedSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
		_renderCompletedFence(CreateManagedRef<VulkanGraphicsFence>(id, true)),
		_cache(CreateUniqueRef<VulkanRenderContextCache>())
	{
		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
		{
			throw std::exception("A graphics queue needs to be created for rendering");
		}

		_commandBuffer = graphicsQueue->Pool.Allocate(true);
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		_imageAvailableSemaphore.Invalidate();
		_renderCompletedSemaphore.Invalidate();
		_renderCompletedFence.Invalidate();

		_cache.reset();

		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);
		if (graphicsQueue && _commandBuffer)
		{
			graphicsQueue->Pool.Free(*_commandBuffer);
		}

		_commandBuffer.reset();
	}

	void VulkanRenderContext::WaitForRenderingToComplete()
	{
		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
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

	void VulkanRenderContext::AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Wait on semaphore was invalid")
			Assert(false)
			return;
		}

		Assert(_vulkanRenderOperation.has_value())

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_vulkanRenderOperation->WaitOnSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Signal semaphore was invalid")
			Assert(false)
			return;
		}

		Assert(_vulkanRenderOperation.has_value())

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_vulkanRenderOperation->RenderCompletedSignalSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::SetShader(const RenderPassShaderData& shader)
	{
		Assert(_vulkanRenderOperation.has_value())

		if (_vulkanRenderOperation->CurrentShaderID.has_value() && _vulkanRenderOperation->CurrentShaderID.value() == shader.ID)
			return;

		// TODO: maybe get the VulkanRenderPassShader here
		_vulkanRenderOperation->CurrentShaderID = shader.ID;

		_renderOperation->InstanceUniforms.Clear();
		_renderOperation->DrawUniforms.Clear();

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Shader);
		_vulkanRenderOperation->BoundInstanceDescriptors.reset();
	}

	void VulkanRenderContext::Draw(const MeshData& mesh)
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(mesh.IndexBuffer.IsValid())
		Assert(mesh.VertexBuffer.IsValid())

		if (!FlushStateChanges())
		{
			CocoError("Failed to setup state for mesh. Skipping...")
			return;
		}

		// Bind the vertex buffer
		std::array<VkDeviceSize, 1> offsets = { 0 };
		VulkanBuffer* vertexBuffer = static_cast<VulkanBuffer*>(mesh.VertexBuffer.Get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		VulkanBuffer* indexBuffer = static_cast<VulkanBuffer*>(mesh.IndexBuffer.Get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), mesh.IndexCount, 1, mesh.FirstIndexOffset, 0, 0);
		_renderOperation->TrianglesDrawn += mesh.IndexCount / 3;
	}

	void VulkanRenderContext::SetBackbufferIndex(uint32 index)
	{
		_backbufferIndex = static_cast<int>(index);
	}

	bool VulkanRenderContext::ResetImpl()
	{
		_backbufferIndex = -1;
		_vulkanRenderOperation.reset();
		_renderCompletedFence->Reset();

		return true;
	}

	bool VulkanRenderContext::BeginImpl()
	{
		try
		{
			std::span<const RenderTarget> rts = _renderOperation->RenderView.GetRenderTargets();
			std::vector<VulkanImage*> vulkanImages(rts.size());

			for (size_t i = 0; i < rts.size(); i++)
			{
				Assert(rts[i].Image.IsValid())
				vulkanImages.at(i) = static_cast<VulkanImage*>(rts[i].Image.Get());
			}

			VulkanRenderPass& renderPass = _device->GetCache()->GetOrCreateRenderPass(_renderOperation->Pipeline);
			VulkanFramebuffer& framebuffer = _cache->GetOrCreateFramebuffer(_renderOperation->RenderView.GetViewportRect().Size, renderPass, vulkanImages);

			// Setup the Vulkan-specific render operation
			_vulkanRenderOperation.emplace(VulkanContextRenderOperation(framebuffer, renderPass));
			_vulkanRenderOperation->WaitOnSemaphores.push_back(_imageAvailableSemaphore);
			_vulkanRenderOperation->RenderCompletedSignalSemaphores.push_back(_renderCompletedSemaphore);

			const RectInt& viewportRect = _renderOperation->RenderView.GetViewportRect();
			_vulkanRenderOperation->ViewportRect = viewportRect;
			_vulkanRenderOperation->ScissorRect = _renderOperation->RenderView.GetScissorRect();

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = renderPass.GetRenderPass();
			beginInfo.framebuffer = framebuffer.GetFramebuffer();

			beginInfo.renderArea.offset.x = static_cast<uint32_t>(viewportRect.Offset.X);
			beginInfo.renderArea.offset.y = static_cast<uint32_t>(viewportRect.Offset.Y); // TODO: check if this is correct
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

		// Submit the command buffer and free it
		_commandBuffer->EndAndSubmit(
			&_vulkanRenderOperation->WaitOnSemaphores,
			&_vulkanRenderOperation->RenderCompletedSignalSemaphores,
			_renderCompletedFence);

		_cache->ResetForNextRender();
	}

	void VulkanRenderContext::UniformChanged(UniformScope scope, ShaderUniformData::UniformKey key)
	{
		Assert(_vulkanRenderOperation.has_value())

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Uniform);
	}

	void VulkanRenderContext::AddPreRenderPassImageTransitions()
	{
		const auto& attachmentFormats = _renderOperation->Pipeline.InputAttachments;

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const AttachmentFormat& attachment = attachmentFormats.at(i);

			// Don't bother transitioning layouts for attachments that aren't preserved between frames since Vulkan will initialize them for us
			if (!attachment.ShouldPreserve)
				continue;

			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			VulkanImage* image = static_cast<VulkanImage*>(_renderOperation->RenderView.GetRenderTarget(i).Image.Get());

			image->TransitionLayout(*_commandBuffer, layout);
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions()
	{
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

			const RenderPassShaderData& boundShaderData = _renderOperation->RenderView.GetRenderPassShaderData(_vulkanRenderOperation->CurrentShaderID.value());
			VulkanRenderPassShader& shader = _device->GetCache()->GetOrCreateShader(boundShaderData.ShaderData);
			VulkanUniformData& uniformData = _cache->GetOrCreateUniformData(shader);
			VulkanPipeline* pipeline = nullptr;

			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Shader))
			{
				pipeline = &_device->GetCache()->GetOrPipeline(
					_vulkanRenderOperation->RenderPass,
					shader,
					_renderOperation->CurrentPassIndex
				);

				_vulkanRenderOperation->BoundPipeline = pipeline;

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

				VkDescriptorSet set = uniformData.PrepareData(VulkanUniformData::sGlobalInstanceID, _renderOperation->GlobalUniforms, shader, true);
				if (set)
				{
					// Bind the global descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline->GetPipelineLayout(),
						sGlobalDescriptorSetIndex, 1, &set,
						0, 0);
				}
			}

			Assert(_vulkanRenderOperation->BoundPipeline.has_value())
			pipeline = _vulkanRenderOperation->BoundPipeline.value();

			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Uniform) ||
				!_vulkanRenderOperation->BoundInstanceDescriptors.has_value())
			{
				// HACK: temporary
				uint64 instanceID = shader.GetInfo().Hash + shader.ID;
				VkDescriptorSet set = uniformData.PrepareData(instanceID, _renderOperation->InstanceUniforms, shader, true);

				if (set)
				{
					VulkanPipeline* pipeline = _vulkanRenderOperation->BoundPipeline.value();

					// Bind the instance descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline->GetPipelineLayout(),
						sInstanceDescriptorSetIndex, 1, &set,
						0, 0);

					_vulkanRenderOperation->BoundInstanceDescriptors = set;
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