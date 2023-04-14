#pragma once

#include <Coco/Rendering/Graphics/Resources/RenderContext.h>

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Core/Types/Optional.h>
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
	class VulkanRenderPass;
	class VulkanPipeline;
	class VulkanSubshader;

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

		Ref<VulkanRenderPass> _renderPass = nullptr;
		Ref<CachedVulkanFramebuffer> _framebuffer = nullptr;

		Set<RenderContextStateChange> _stateChanges;
		ResourceID _currentShader = Resource::InvalidID;
		ResourceID _currentMaterial = Resource::InvalidID;
		Ref<VulkanPipeline> _currentPipeline = nullptr;
		UnorderedMap<ResourceID, VkDescriptorSet> _materialDescriptorSets;

		int _backbufferIndex = -1;

	public:
		RenderContextVulkan(GraphicsDevice* device);
		~RenderContextVulkan() final;

		void SetViewport(const RectInt& rect) final;
		void UseShader(ResourceID shaderID) final;
		void UseMaterial(ResourceID materialID) final;
		void Draw(const ObjectRenderData& objectData) final;
		bool IsAvaliableForRendering() noexcept final { return _renderingCompleteFence->IsSignalled(); }
		void WaitForRenderingCompleted() final;

		/// @brief Gets the Vulkan semaphore that should be used to signal when the backbuffer is available
		/// @return The image available semaphore
		VkSemaphore GetImageAvailableSemaphore() const noexcept { return _imageAvailableSemaphore->GetSemaphore(); }

		/// @brief Gets the Vulkan semaphore that is signaled when rendering has completed
		/// @return The render complete semaphore
		VkSemaphore GetRenderCompleteSemaphore() const noexcept { return _renderingCompleteSemaphore->GetSemaphore(); }

		/// @brief Sets the index of the backbuffer image being used
		/// @param backbufferIndex The index of the backbuffer image being used
		constexpr void SetBackbufferIndex(int backbufferIndex) { _backbufferIndex = backbufferIndex; }

		/// @brief Gets the index of the backbuffer image being used (if it was set)
		/// @return The index of the backbuffer image being used
		constexpr int GetBackbufferIndex() const { return _backbufferIndex; }

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
		/// @param subshader The subshader to use
		/// @param materialID The ID of the material to use
		/// @param set Will be filled out with the descriptor set
		/// @return True if the descriptor set was created
		bool GetOrAllocateMaterialDescriptorSet(const Ref<VulkanSubshader>& subshader, ResourceID materialID, VkDescriptorSet& set);

		/// @brief Event handler for the device purging resources
		/// @return If the event was handled
		bool HandlePurgeResources();

		/// @brief Adds any necessary pre-render pass image transitions to the render targets
		void AddPreRenderPassImageTransitions();

		/// @brief Adds any necessary post-render pass image transitions to the render targets
		void AddPostRenderPassImageTransitions();
	};
}