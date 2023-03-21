#pragma once

#include <Coco/Rendering/Graphics/Resources/RenderContext.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Set.h>
#include "../VulkanRenderCache.h"
#include "../VulkanIncludes.h"
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"
#include "RenderContextVulkanCache.h"

namespace Coco::Rendering
{
	class RenderingService;
	class RenderPipeline;
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class ImageVulkan;
	class RenderCacheVulkan;

	/// <summary>
	/// A cached Vulkan framebuffer
	/// </summary>
	struct CachedVulkanFramebuffer final : public CachedResource
	{
		GraphicsDeviceVulkan* Device;
		WeakRef<RenderPipeline> PipelineRef;
		SizeInt FramebufferSize = SizeInt::Zero;
		VkFramebuffer Framebuffer = nullptr;

		CachedVulkanFramebuffer(GraphicsDeviceVulkan* device, Ref<RenderPipeline> pipeline);
		virtual ~CachedVulkanFramebuffer();

		bool IsInvalid() const noexcept final { return PipelineRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// <summary>
		/// Checks if this cached framebuffer needs to be updated
		/// </summary>
		/// <param name="framebufferSize">The desired size of the framebuffer</param>
		/// <returns>True if this resource should be updated</returns>
		bool NeedsUpdate(const SizeInt& framebufferSize) const noexcept
		{
			return NeedsUpdate() || FramebufferSize != framebufferSize;
		}

		/// <summary>
		/// Destroys the framebuffer
		/// </summary>
		void DestroyFramebuffer() noexcept;
	};

	/// <summary>
	/// Vulkan-implementation of a RenderContext
	/// </summary>
	class RenderContextVulkan final : public RenderContext
	{
	private:
		static constexpr uint s_globalDescriptorSetIndex = 0;
		static constexpr uint s_materialDescriptorSetIndex = 1;
		static constexpr uint64_t s_staleTickCount = 500;

		RenderingService* _renderingService;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;
		CommandBufferVulkan* _commandBuffer;
		WeakManagedRef<GraphicsSemaphoreVulkan> _imageAvailableSemaphore;
		WeakManagedRef<GraphicsSemaphoreVulkan> _renderingCompleteSemaphore;
		WeakManagedRef<GraphicsFenceVulkan> _renderingCompleteFence;
		Managed<RenderContextVulkanCache> _renderCache;

		RenderContextState _currentState;
		List<WeakManagedRef<GraphicsSemaphoreVulkan>> _signalSemaphores;
		List<WeakManagedRef<GraphicsSemaphoreVulkan>> _waitSemaphores;

		WeakManagedRef<BufferVulkan> _globalUBO;
		VulkanDescriptorLayout _globalDescriptor;
		WeakManagedRef<VulkanDescriptorPool> _globalDescriptorPool;
		VkDescriptorSet _globalDescriptorSet;

		Set<RenderContextStateChange> _stateChanges;
		Ref<CachedVulkanRenderPass> _renderPass = nullptr;
		Ref<CachedVulkanFramebuffer> _framebuffer = nullptr;
		List<WeakManagedRef<ImageVulkan>> _renderTargets;
		Ref<CachedVulkanPipeline> _currentPipeline = nullptr;

		Ref<Shader> _currentShader;
		Ref<Material> _currentMaterial;
		Map<ResourceID, VkDescriptorSet> _materialDescriptorSets;

	public:
		RenderContextVulkan(GraphicsDevice* device);
		~RenderContextVulkan() final;

		void SetViewport(const RectInt& rect) final;
		void UseShader(Ref<Shader> shader) final;
		void UseMaterial(Ref<Material> material) final;
		void Draw(const Mesh* mesh, const Matrix4x4& modelMatrix) final;
		bool IsAvaliableForRendering() noexcept final { return _renderingCompleteFence->IsSignalled(); }
		void WaitForRenderingCompleted() final;

		/// <summary>
		/// Sets the render targets for this render context to use
		/// </summary>
		/// <param name="renderTargets">The render targets to use</param>
		void SetRenderTargets(const List<WeakManagedRef<ImageVulkan>>& renderTargets);

		/// <summary>
		/// Gets the render targets that this context is using
		/// </summary>
		/// <returns>The render targets that this context is using</returns>
		List<WeakManagedRef<ImageVulkan>> GetRenderTargets() const noexcept { return _renderTargets; }

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
		/// Ensures a framebuffer is created for the current pipeline and render pass using the current render targets
		/// </summary>
		void EnsureFramebufferUpdated();

		/// <summary>
		/// Creates the global descriptor set
		/// </summary>
		void CreateGlobalDescriptorSet();

		/// <summary>
		/// Creates a descriptor set for the currently bound material instance
		/// </summary>
		/// <param name="set">The descriptor set for the currently bound material</param>
		/// <returns>True if the descriptor set was successfully retrieved</param>
		bool GetOrAllocateMaterialDescriptorSet(const string& subshaderName, const Ref<Shader>& shader, const Ref<Material>& material, VkDescriptorSet& set);

		/// <summary>
		/// Event handler for the device purging resources
		/// </summary>
		/// <returns>If the event was handled</returns>
		bool HandlePurgeResources();
	};
}