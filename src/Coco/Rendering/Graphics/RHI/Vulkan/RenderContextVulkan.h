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
	class RenderingService;
	class RenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class ImageVulkan;

	/// <summary>
	/// Vulkan-implementation of a RenderContext
	/// </summary>
	class RenderContextVulkan final : public RenderContext
	{
	private:
		RenderingService* _renderingService;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;
		CommandBufferVulkan* _commandBuffer;
		GraphicsResourceRef<BufferVulkan> _globalUBO;
		GraphicsResourceRef<GraphicsSemaphoreVulkan> _imageAvailableSemaphore;
		GraphicsResourceRef<GraphicsSemaphoreVulkan> _renderingCompleteSemaphore;
		GraphicsResourceRef<GraphicsFenceVulkan> _renderingCompleteFence;

		WeakRef<RenderPipeline> _framebufferPipeline;
		List<GraphicsResourceRef<ImageVulkan>> _renderTargets;

		RenderContextState _currentState;
		List<GraphicsResourceRef<GraphicsSemaphoreVulkan>> _signalSemaphores;
		List<GraphicsResourceRef<GraphicsSemaphoreVulkan>> _waitSemaphores;

		Set<RenderContextStateChange> _stateChanges;
		VulkanRenderPass _renderPass;
		VkFramebuffer _framebuffer = nullptr;
		VulkanPipeline _currentPipeline = VulkanPipeline::None;
		Ref<Shader> _currentShader;
		Ref<Material> _currentMaterial;

		VulkanDescriptorLayout _globalDescriptor;
		GraphicsResourceRef<DescriptorPoolVulkan> _globalDescriptorPool;
		VkDescriptorSet _globalDescriptorSet;

		GraphicsResourceRef<BufferVulkan> _objectUBO;
		uint64_t _currentObjectUBOOffset = 0;
		Map<ResourceID, GraphicsResourceRef<DescriptorPoolVulkan>> _shaderDescriptorPools;
		Map<ResourceID, VkDescriptorSet> _materialDescriptorSets;

	public:
		RenderContextVulkan(GraphicsDevice* owningDevice);
		~RenderContextVulkan() final;

		void SetViewport(const Vector2Int& offset, const SizeInt& size) final;
		void UseShader(Ref<Shader> shader) final;
		void UseMaterial(Ref<Material> material) final;
		void Draw(const Mesh* mesh, const Matrix4x4& modelMatrix) final;
		bool IsAvaliableForRendering() noexcept final { return _renderingCompleteFence->IsSignalled(); }
		void WaitForRenderingCompleted() final;

		/// <summary>
		/// Sets the render targets for this render context to use
		/// </summary>
		/// <param name="renderTargets">The render targets to use</param>
		void SetRenderTargets(const List<GraphicsResourceRef<ImageVulkan>>& renderTargets);

		/// <summary>
		/// Gets the render targets that this context is using
		/// </summary>
		/// <returns>The render targets that this context is using</returns>
		List<GraphicsResourceRef<ImageVulkan>> GetRenderTargets() const noexcept { return _renderTargets; }

		/// <summary>
		/// Gets the render pass being used for this context
		/// </summary>
		/// <returns>The render pass being used</returns>
		VulkanRenderPass GetRenderPass() const noexcept { return _renderPass; }

		/// <summary>
		/// Gets the Vulkan semaphore that should be used to signal when the backbuffer is available
		/// </summary>
		/// <returns>The image available semaphore</returns>
		VkSemaphore GetImageAvailableSemaphore() const noexcept { return _imageAvailableSemaphore->GetSemaphore(); }

		/// <summary>
		/// Gets the Vulkan semaphore that is signaled when rendering has completed
		/// </summary>
		/// <returns>The render complete semaphore</returns>
		VkSemaphore GetRenderCompleteSemaphore() const noexcept { return _renderingCompleteSemaphore->GetSemaphore(); }

	protected:
		virtual bool BeginImpl() final;
		virtual void EndImpl() final;
		virtual void ResetImpl() final;

	private:
		/// <summary>
		/// Attempts to flush all state changes and bind the current state
		/// </summary>
		bool FlushStateChanges();

		/// <summary>
		/// Creates the framebuffer from the current renderpass and render view
		/// </summary>
		void CreateFramebuffer();

		/// <summary>
		/// Destroys the current framebuffer
		/// </summary>
		void DestroyFramebuffer() noexcept;

		/// <summary>
		/// Creates the global descriptor set
		/// </summary>
		void CreateGlobalDescriptorSet();

		/// <summary>
		/// Creates a descriptor set for the currently bound material instance
		/// </summary>
		VkDescriptorSet GetOrAllocateMaterialDescriptorSet();
	};
}