#include "RenderContextVulkan.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Shader.h>
#include "../VulkanUtilities.h"
#include "../GraphicsDeviceVulkan.h"
#include "../GraphicsPlatformVulkan.h"
#include "Cache/VulkanShader.h"
#include "Cache/VulkanRenderPass.h"
#include "Cache/VulkanPipeline.h"
#include "Cache/VulkanFramebuffer.h"
#include "Cache/VulkanShaderResource.h"
#include "../VulkanRenderCache.h"
#include "ImageVulkan.h"
#include "ImageSamplerVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"
#include "../CommandBufferPoolVulkan.h"
#include <Coco/Rendering/RenderingUtilities.h>

namespace Coco::Rendering::Vulkan
{
	RenderContextVulkan::RenderContextVulkan(const ResourceID& id, const string& name) :
		GraphicsResource<GraphicsDeviceVulkan, RenderContext>(id, name), 
		_currentState(RenderContextState::Ready)
	{
		if (!_device->GetGraphicsCommandPool(_pool))
			throw VulkanRenderingException("A graphics command pool needs to be created for rendering");

		_commandBuffer = _pool->Allocate(true);

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>(FormattedString("{} Image Available Semaphore", name));
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>(FormattedString("{} Rendering Complete Semaphore", name));
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(FormattedString("{} Rendering Complete Fence", name), true);

		_renderCache = CreateManagedRef<RenderContextVulkanCache>();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->WaitForIdle();

		_renderCache.Reset();
		_currentFramebuffer = nullptr;

		_frameSignalSemaphores.Clear();
		_frameWaitSemaphores.Clear();

		_pool->Free(_commandBuffer);
		_commandBuffer = Ref<CommandBufferVulkan>();

		_device->PurgeResource(_imageAvailableSemaphore);
		_device->PurgeResource(_renderingCompleteSemaphore);
		_device->PurgeResource(_renderingCompleteFence);
	}

	void RenderContextVulkan::SetViewport(const RectInt& rect)
	{
		VkViewport viewport = {};
		viewport.x = static_cast<float>(rect.Offset.X);
		viewport.y = static_cast<float>(-rect.Offset.Y);
		viewport.width = static_cast<float>(rect.Size.Width);
		viewport.height = static_cast<float>(rect.Size.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = static_cast<uint32_t>(rect.Size.Width);
		scissor.extent.height = static_cast<uint32_t>(rect.Size.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void RenderContextVulkan::Draw(const ObjectRenderData& objectData)
	{
		MeshRenderData& meshData = _currentRenderView->Meshs.at(objectData.MeshData);

		// Sanity checks
		if (!meshData.VertexBuffer.IsValid() || !meshData.IndexBuffer.IsValid())
		{
			LogError(_device->GetLogger(), "Mesh has no index and/or vertex buffer. Skipping...");
			return;
		}

		if (meshData.VertexCount == 0 || meshData.IndexCount == 0)
		{
			LogError(_device->GetLogger(), "Mesh has no index and/or vertex data. Skipping...");
			return;
		}

		// Flush the pipeline state
		if (!FlushStateChanges())
		{
			LogError(_device->GetLogger(), "Failed setting up state for rendering mesh. Skipping...");
			return;
		}

		// Bind the vertex buffer
		Array<VkDeviceSize, 1> offsets = { 0 };
		BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(meshData.VertexBuffer.Get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(meshData.IndexBuffer.Get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		ApplyPushConstants();

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(meshData.IndexCount), 1, 0, 0, 0);

		_currentTrianglesDrawn += meshData.IndexCount / 3;
	}

	void RenderContextVulkan::WaitForRenderingCompleted()
	{
		_renderingCompleteFence->Wait(Math::MaxValue<uint64_t>());
	}

	bool RenderContextVulkan::BeginImpl()
	{
		try
		{
			if (!_currentRenderPipeline)
				throw VulkanRenderingException("No RenderPipline was set");

			_currentVulkanRenderPass = _device->GetRenderCache()->GetOrCreateRenderPass(_currentRenderPipeline);

			_currentFramebuffer = _renderCache->GetOrCreateFramebuffer(_currentRenderView, *_currentVulkanRenderPass, _currentRenderPipeline);

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			VkRenderPassBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = _currentVulkanRenderPass->GetRenderPass();
			beginInfo.framebuffer = _currentFramebuffer->GetFramebuffer();

			beginInfo.renderArea.offset.x =	static_cast<uint32_t>(_currentRenderView->ViewportRect.Offset.X);
			beginInfo.renderArea.offset.y =	static_cast<uint32_t>(-_currentRenderView->ViewportRect.Offset.Y);
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(_currentRenderView->ViewportRect.Size.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(_currentRenderView->ViewportRect.Size.Height);

			List<VkClearValue> clearValues(_currentRenderView->RenderTargets.Count());
			const VkClearDepthStencilValue depthStencilClearValue{ 1.0f, 0 };

			// Set clear clear color for each render target
			for (int i = 0; i < clearValues.Count(); i++)
			{
				const RenderTarget& rt = _currentRenderView->RenderTargets[i];
				if (IsDepthStencilFormat(rt.Image->GetDescription().PixelFormat))
				{
					clearValues[i].depthStencil = depthStencilClearValue;
				}
				else
				{
					clearValues[i].color = 
					{ 
						static_cast<float>(rt.ClearColor.R), 
						static_cast<float>(rt.ClearColor.G), 
						static_cast<float>(rt.ClearColor.B),
						static_cast<float>(rt.ClearColor.A) 
					};
				}				
			}

			beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.Count());
			beginInfo.pClearValues = clearValues.Data();

			vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

			_currentState = RenderContextState::CompilingDrawCalls;

			return true;
		}
		catch (const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Unable to begin VulkanRenderContext: {}", ex.what()));
			return false;
		}
	}

	bool RenderContextVulkan::BeginPassImpl()
	{
		if (_currentRenderPassIndex > 0)
		{
			vkCmdNextSubpass(_commandBuffer->GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		return true;
	}

	void RenderContextVulkan::EndImpl()
	{
		try
		{
			// Flush uniform data changes
			_renderCache->FlushUniformBufferChanges();

			List<GraphicsSemaphore*> waitSemaphores;

			for (Ref<GraphicsSemaphoreVulkan>& semaphore : _frameWaitSemaphores)
				if(semaphore.IsValid())
					waitSemaphores.Add(semaphore.Get());

			List<GraphicsSemaphore*> signalSemaphores;

			for (Ref<GraphicsSemaphoreVulkan>& semaphore : _frameSignalSemaphores)
				if (semaphore.IsValid())
					signalSemaphores.Add(semaphore.Get());

			vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());

			AddPostRenderPassImageTransitions();

			_commandBuffer->EndAndSubmit(&waitSemaphores, &signalSemaphores, _renderingCompleteFence.Get());

			_currentState = RenderContextState::DrawCallsSubmitted;
		}
		catch(const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Unable to end VulkanRenderContext: {}", ex.what()));
		}

		_currentPipeline = nullptr;
		_currentVulkanRenderPass = nullptr;
	}

	void RenderContextVulkan::ResetImpl()
	{
		// Free all material descriptor sets
		_renderCache->ResetForNewFrame();

		// Clear all currently bound items
		_stateChanges.clear();
		_currentPipeline = nullptr;
		ResetShaderUniformData();

		// Reset render syncronization objects
		_frameSignalSemaphores.Clear();
		_frameWaitSemaphores.Clear();

		_frameSignalSemaphores.Add(_renderingCompleteSemaphore);
		_frameWaitSemaphores.Add(_imageAvailableSemaphore);
		_renderingCompleteFence->Reset();

		_currentState = RenderContextState::Ready;
	}

	void RenderContextVulkan::NewShaderBound()
	{
		_stateChanges.insert(RenderContextStateChange::Shader);
		_currentPipeline = nullptr;
	}

	void RenderContextVulkan::NewMaterialBound()
	{
		_stateChanges.insert(RenderContextStateChange::Material);
	}

	void RenderContextVulkan::UniformUpdated(const string& name)
	{
		_stateChanges.insert(RenderContextStateChange::Uniform);
	}

	void RenderContextVulkan::TextureSamplerUpdated(const string& name)
	{
		_stateChanges.insert(RenderContextStateChange::Texture);
	}

	bool RenderContextVulkan::FlushStateChanges()
	{
		bool stateBound = false;

		try
		{
			if (!_currentVulkanRenderPass)
				throw RenderingException("A render pass was not set");

			// We need to have a shader bound to draw anything
			if (_currentShader.ID == Resource::InvalidID)
				throw RenderingException("No shader was bound");

			VulkanShader* shader = _device->GetRenderCache()->GetOrCreateVulkanShader(_currentShader);
			const string subshaderName = _currentRenderPass->GetSubshaderName();

			const VulkanSubshader* vulkanSubshader;
			if (!shader->TryGetSubshader(subshaderName, vulkanSubshader))
				throw RenderingException(FormattedString("Shader {} has no subshader named {}", shader->GetName(), subshaderName));

			// Get the shader resource
			VulkanShaderResource* shaderResource = _renderCache->GetOrCreateShaderResource(*shader);

			if (_stateChanges.contains(RenderContextStateChange::Shader))
			{
				_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(
					*_currentVulkanRenderPass,
					*shader,
					_currentRenderPass->GetSubshaderName(),
					static_cast<uint32_t>(_currentRenderPassIndex));

				// Ensure a pipeline state has been bound
				if (_currentPipeline == nullptr)
					throw RenderingException("A pipeline was not bound");

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline->GetPipeline());

				VkDescriptorSet set = shaderResource->PrepareGlobalData(*vulkanSubshader, *_currentRenderView.Get(), _currentGlobalUniformData);

				if (set != nullptr)
				{
					// Bind the global descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						_currentPipeline->GetPipelineLayout(),
						_globalDescriptorSetIndex, 1, &set,
						0, 0);
				}
			}

			// Ensure a pipeline state has been bound
			if (_currentPipeline == nullptr)
				throw RenderingException("A pipeline was not bound");

			if (_stateChanges.contains(RenderContextStateChange::Material) || 
				_stateChanges.contains(RenderContextStateChange::Uniform) || 
				_stateChanges.contains(RenderContextStateChange::Texture))
			{
				VkDescriptorSet set = shaderResource->PrepareInstanceData(*vulkanSubshader, *_currentRenderView.Get(), _currentInstanceUniformData);

				if (set != nullptr)
				{
					// Bind the instance descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						_currentPipeline->GetPipelineLayout(),
						_instanceDescriptorSetIndex, 1, &set,
						0, 0);
				}
			}

			stateBound = true;
			_currentDrawCallCount++;
		}
		catch (const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Failed binding pipeline state: {}", ex.what()));
			stateBound = false;
		}

		_stateChanges.clear();
		return stateBound;
	}

	void RenderContextVulkan::ApplyPushConstants()
	{
		auto it = _currentShader.Subshaders.find(_currentRenderPass->GetSubshaderName());
		if (it == _currentShader.Subshaders.end())
		{
			LogError(_device->GetLogger(), FormattedString(
				"Failed setting push constants. Shader {} has no subshader for pass {}", 
				_currentShader.ID.ToString(), 
				_currentRenderPass->GetSubshaderName()
			));
			return;
		}

		List<ShaderUniformDescriptor> uniforms = it->second.GetScopedUniforms(ShaderDescriptorScope::Draw);

		const uint minimumAlignment = _device->GetMinimumBufferAlignment();
		const uint32_t alignedFloatSize = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Float), minimumAlignment));
		const uint32_t alignedVec2Size = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector2), minimumAlignment));
		const uint32_t alignedVec3Size = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector3), minimumAlignment));
		const uint32_t alignedVec4Size = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4), minimumAlignment));
		const uint32_t alignedIntSize = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Int), minimumAlignment));
		const uint32_t alignedVec2IntSize = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector2Int), minimumAlignment));
		const uint32_t alignedVec3IntSize = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector3Int), minimumAlignment));
		const uint32_t alignedVec4IntSize = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4Int), minimumAlignment));
		const uint32_t alignedMat4Size = static_cast<uint32_t>(RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Matrix4x4), minimumAlignment));

		Array<float, 16> tempData = { 0.0f };
		Array<int32_t, 4> tempIntData = { 0 };

		uint32_t offset = 0;
		for (int i = 0; i < uniforms.Count(); i++)
		{
			const ShaderUniformDescriptor& uniform = uniforms[i];
			VkShaderStageFlagBits stageFlags = ToVkShaderStageFlagBits(uniform.BindingPoints);

			switch (uniform.Type)
			{
			case BufferDataFormat::Int:
			{
				int32_t v;

				if (_currentDrawUniformData.Ints.contains(uniform.Name))
					v = _currentDrawUniformData.Ints.at(uniform.Name);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(int32_t), &v);

				offset += alignedIntSize;
				break;
			}
			case BufferDataFormat::Vector2Int:
			{
				Vector2Int vec2;

				if (_currentDrawUniformData.Vector2Ints.contains(uniform.Name))
					vec2 = _currentDrawUniformData.Vector2Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec2.X);
				tempIntData[1] = static_cast<int32_t>(vec2.Y);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(int32_t) * 2, tempIntData.data());

				offset += alignedVec2IntSize;
				break;
			}
			case BufferDataFormat::Vector3Int:
			{
				Vector3Int vec3;

				if (_currentDrawUniformData.Vector3Ints.contains(uniform.Name))
					vec3 = _currentDrawUniformData.Vector3Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec3.X);
				tempIntData[1] = static_cast<int32_t>(vec3.Y);
				tempIntData[2] = static_cast<int32_t>(vec3.Z);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(int32_t) * 3, tempIntData.data());

				offset += alignedVec3IntSize;
				break;
			}
			case BufferDataFormat::Vector4Int:
			{
				Vector4Int vec4;

				if (_currentDrawUniformData.Vector4Ints.contains(uniform.Name))
					vec4 = _currentDrawUniformData.Vector4Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec4.X);
				tempIntData[1] = static_cast<int32_t>(vec4.Y);
				tempIntData[2] = static_cast<int32_t>(vec4.Z);
				tempIntData[3] = static_cast<int32_t>(vec4.W);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(int32_t) * 4, tempIntData.data());

				offset += alignedVec4IntSize;
				break;
			}
			case BufferDataFormat::Float:
			{
				float v;

				if (_currentDrawUniformData.Floats.contains(uniform.Name))
					v = _currentDrawUniformData.Floats.at(uniform.Name);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float), &v);

				offset += alignedFloatSize;
				break;
			}
			case BufferDataFormat::Vector2:
			{
				Vector2 vec2;

				if (_currentDrawUniformData.Vector2s.contains(uniform.Name))
					vec2 = _currentDrawUniformData.Vector2s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec2.X);
				tempData[1] = static_cast<float>(vec2.Y);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float) * 2, tempData.data());

				offset += alignedVec2Size;
				break;
			}
			case BufferDataFormat::Vector3:
			{
				Vector3 vec3;

				if (_currentDrawUniformData.Vector3s.contains(uniform.Name))
					vec3 = _currentDrawUniformData.Vector3s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec3.X);
				tempData[1] = static_cast<float>(vec3.Y);
				tempData[2] = static_cast<float>(vec3.Z);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float) * 3, tempData.data());

				offset += alignedVec3Size;
				break;
			}
			case BufferDataFormat::Vector4:
			{
				Vector4 vec4;

				if (_currentDrawUniformData.Vector4s.contains(uniform.Name))
					vec4 = _currentDrawUniformData.Vector4s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec4.X);
				tempData[1] = static_cast<float>(vec4.Y);
				tempData[2] = static_cast<float>(vec4.Z);
				tempData[3] = static_cast<float>(vec4.W);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float) * 4, tempData.data());

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Color:
			{
				Color c;

				if (_currentDrawUniformData.Colors.contains(uniform.Name))
					c = _currentDrawUniformData.Colors.at(uniform.Name).AsLinear();

				tempData[0] = static_cast<float>(c.R);
				tempData[1] = static_cast<float>(c.G);
				tempData[2] = static_cast<float>(c.B);
				tempData[3] = static_cast<float>(c.A);

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float) * 4, tempData.data());

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Matrix4x4:
			{
				Matrix4x4 mat4;

				if (_currentDrawUniformData.Matrix4x4s.contains(uniform.Name))
					mat4 = _currentDrawUniformData.Matrix4x4s.at(uniform.Name);

				tempData = mat4.AsFloat();

				vkCmdPushConstants(
					_commandBuffer->GetCmdBuffer(),
					_currentPipeline->GetPipelineLayout(),
					stageFlags,
					offset, sizeof(float) * 16, tempData.data());

				offset += alignedMat4Size;
				break;
			}
			default:
				break;
			}
		}
	}

	void RenderContextVulkan::AddPreRenderPassImageTransitions()
	{
		const auto& pipelineAttachments = _currentRenderPipeline->GetPipelineAttachmentDescriptions();

		for (uint64_t i = 0; i < _currentRenderView->RenderTargets.Count(); i++)
		{
			// Don't bother transitioning layouts for attachments that aren't preserved
			if (!pipelineAttachments[i].Description.ShouldPreserve)
				continue;

			Ref<Image>& rt = _currentRenderView->RenderTargets[i].Image;

			VkImageLayout layout = ToAttachmentLayout(pipelineAttachments[i].Description.PixelFormat);

			ImageVulkan* image = static_cast<ImageVulkan*>(rt.Get());
			image->TransitionLayout(
				*_commandBuffer.Get(),
				layout
			);
		}
	}

	void RenderContextVulkan::AddPostRenderPassImageTransitions()
	{
		const auto& pipelineAttachments = _currentRenderPipeline->GetPipelineAttachmentDescriptions();

		for (uint64_t i = 0; i < _currentRenderView->RenderTargets.Count(); i++)
		{
			Ref<Image>& rt = _currentRenderView->RenderTargets[i].Image;

			VkImageLayout layout = ToAttachmentLayout(pipelineAttachments[i].Description.PixelFormat);

			ImageVulkan* image = static_cast<ImageVulkan*>(rt.Get());
			image->SetCurrentLayout(layout);

			if ((rt->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
			{
				image->TransitionLayout(
					*_commandBuffer.Get(),  
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				);
			}
		}
	}
}