#pragma once

#include <Coco/Rendering/Graphics/Resources/RenderContext.h>

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Set.h>
#include "../VulkanIncludes.h"
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "VulkanDescriptorSet.h"

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
	class RenderContextVulkanCache;
	class CommandBufferPoolVulkan;
	class CommandBufferVulkan;
	class BufferVulkan;
	class VulkanDescriptorPool;
	struct CachedVulkanRenderPass;
	struct CachedVulkanPipeline;

	/// @brief A cached Vulkan framebuffer
	struct CachedVulkanFramebuffer final : public CachedResource
	{
		/// @brief A pointer to a Vulkan graphics device
		GraphicsDeviceVulkan* const Device;
		
		/// @brief The pipeline that this framebuffer was created from
		WeakRef<RenderPipeline> PipelineRef;

		/// @brief The size of the framebuffer
		SizeInt FramebufferSize = SizeInt::Zero;

		/// @brief The Vulkan framebuffer
		VkFramebuffer Framebuffer = nullptr;

		CachedVulkanFramebuffer(GraphicsDeviceVulkan* device, Ref<RenderPipeline> pipeline);
		virtual ~CachedVulkanFramebuffer();

		bool IsInvalid() const noexcept final { return PipelineRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Checks if this cached framebuffer needs to be updated
		/// @param framebufferSize The desired size of the framebuffer
		/// @return True if this resource should be updated
		bool NeedsUpdate(const SizeInt& framebufferSize) const noexcept
		{
			return NeedsUpdate() || FramebufferSize != framebufferSize;
		}

		/// @brief Destroys the framebuffer
		void DestroyFramebuffer() noexcept;
	};

	/// @brief Vulkan-implementation of a RenderContext
	class RenderContextVulkan final : public RenderContext
	{
	private:
		static constexpr uint s_globalDescriptorSetIndex = 0;
		static constexpr uint s_materialDescriptorSetIndex = 1;
		static constexpr uint64_t s_staleTickCount = 500;

		RenderingService* _renderingService;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;
		WeakManagedRef<CommandBufferVulkan> _commandBuffer;
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

		/// @brief Sets the render targets for this render context to use
		/// @param renderTargets The render targets to use
		void SetRenderTargets(const List<WeakManagedRef<ImageVulkan>>& renderTargets);

		/// @brief Gets the render targets that this context is using
		/// @return The render targets that this context is using
		const List<WeakManagedRef<ImageVulkan>>& GetRenderTargets() const noexcept { return _renderTargets; }

		/// @brief Gets the Vulkan semaphore that should be used to signal when the backbuffer is available
		/// @return The image available semaphore
		VkSemaphore GetImageAvailableSemaphore() const noexcept { return _imageAvailableSemaphore->GetSemaphore(); }

		/// @brief Gets the Vulkan semaphore that is signaled when rendering has completed
		/// @return The render complete semaphore
		VkSemaphore GetRenderCompleteSemaphore() const noexcept { return _renderingCompleteSemaphore->GetSemaphore(); }

	protected:
		virtual bool BeginImpl() final;
		virtual void EndImpl() final;
		virtual void ResetImpl() final;

	private:
		/// @brief Attempts to flush all state changes and bind the current state
		/// @return True if the state is bound
		bool FlushStateChanges();

		/// @brief Ensures a framebuffer is created for the current pipeline and render pass using the current render targets
		void EnsureFramebufferUpdated();

		/// @brief Creates the global descriptor set
		void CreateGlobalDescriptorSet();

		/// @brief Creates a descriptor set for the currently bound material instance
		/// @param subshaderName The name of the subshader to use
		/// @param shader The shader to use
		/// @param material The material to use
		/// @param set Will be filled out with the descriptor set
		/// @return True if the descriptor set was created
		bool GetOrAllocateMaterialDescriptorSet(const string& subshaderName, const Ref<Shader>& shader, const Ref<Material>& material, VkDescriptorSet& set);

		/// @brief Event handler for the device purging resources
		/// @return If the event was handled
		bool HandlePurgeResources();
	};
}