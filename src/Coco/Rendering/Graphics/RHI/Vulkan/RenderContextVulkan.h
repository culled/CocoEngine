#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/RenderContext.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Set.h>
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanRenderCache.h"
#include "DescriptorPoolVulkan.h"
#include "BufferVulkan.h"

#include <Coco/Rendering/Graphics/RenderContextTypes.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	class GraphicsDeviceVulkan;
	class ImageVulkan;

	/// <summary>
	/// Vulkan-implementation of a RenderContext
	/// </summary>
	class RenderContextVulkan : public RenderContext
	{
	private:
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;
		CommandBufferVulkan* _commandBuffer;
		BufferVulkan* _globalUBO;
		GraphicsSemaphoreVulkan* _imageAvailableSemaphore;
		GraphicsSemaphoreVulkan* _renderingCompleteSemaphore;
		GraphicsFenceVulkan* _renderingCompleteFence;
		std::hash<RenderContextVulkan*> _rcHash;

		WeakRef<RenderPipeline> _framebufferPipeline;
		List<ImageVulkan*> _renderTargets;

		RenderContextState _currentState;
		List<GraphicsSemaphoreVulkan*> _signalSemaphores;
		List<GraphicsSemaphoreVulkan*> _waitSemaphores;

		Set<RenderContextStateChange> _stateChanges;
		VulkanRenderPass _renderPass;
		VkFramebuffer _framebuffer = nullptr;
		VulkanPipeline _currentPipeline = VulkanPipeline::None;
		Ref<Shader> _currentShader;

		Map<Shader*, DescriptorPoolVulkan*> _descriptorPools;
		Map<Shader*, VkDescriptorSet> _descriptorSets;

	public:
		RenderContextVulkan(GraphicsDevice* owningDevice);
		virtual ~RenderContextVulkan() override;

		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) override;
		virtual void UseShader(Ref<Shader> shader) override;
		virtual void DrawIndexed(uint indexCount, uint indexOffset, uint vertexOffset, uint instanceCount, uint instanceOffset) override;
		virtual void Draw(const Ref<Mesh>& mesh, const Matrix4x4& modelMatrix) override;
		virtual bool IsAvaliableForRendering() override;
		virtual void WaitForRenderingCompleted() override;

		/// <summary>
		/// Sets the render targets for this render context to use
		/// </summary>
		/// <param name="renderTargets">The render targets to use</param>
		void SetRenderTargets(const List<ImageVulkan*>& renderTargets);

		List<ImageVulkan*> GetRenderTargets() const { return _renderTargets; }

		/// <summary>
		/// Gets the render pass being used for this context
		/// </summary>
		/// <returns>The render pass being used</returns>
		VulkanRenderPass GetRenderPass() const { return _renderPass; }

		VkSemaphore GetImageAvailableSemaphore() const { return _imageAvailableSemaphore->GetSemaphore(); }
		VkSemaphore GetRenderCompleteSemaphore() const { return _renderingCompleteSemaphore->GetSemaphore(); }

	protected:
		virtual bool BeginImpl() override;
		virtual void EndImpl() override;
		virtual void ResetImpl() override;

	private:
		/// <summary>
		/// Flushes all state changes and binds the current state
		/// </summary>
		void FlushStateChanges();

		/// <summary>
		/// Creates the framebuffer from the current renderpass and render view
		/// </summary>
		void CreateFramebuffer();

		/// <summary>
		/// Destroys the current framebuffer
		/// </summary>
		void DestroyFramebuffer();
		/// <summary>
		/// Creates a descriptor set for the currently bound shader
		/// </summary>
		void CreateDescriptorSet();
	};
}