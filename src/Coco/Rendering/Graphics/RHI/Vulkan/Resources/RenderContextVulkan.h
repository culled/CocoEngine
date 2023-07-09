#pragma once

#include <Coco/Rendering/Graphics/Resources/RenderContext.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Core/Types/Optional.h>
#include "../VulkanIncludes.h"
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "../VulkanDescriptorSet.h"
#include "../RenderContextVulkanCache.h"

namespace Coco::Rendering
{
	class RenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class ImageVulkan;
	class RenderCacheVulkan;
	class CommandBufferPoolVulkan;
	class CommandBufferVulkan;
	class BufferVulkan;
	class VulkanDescriptorPool;
	class VulkanRenderPass;
	class VulkanPipeline;
	class VulkanShader;
	class VulkanFramebuffer;

	/// @brief Vulkan-implementation of a RenderContext
	class RenderContextVulkan final : public GraphicsResource<GraphicsDeviceVulkan, RenderContext>
	{
	private:
		static constexpr uint _globalDescriptorSetIndex = 0;
		static constexpr uint _materialDescriptorSetIndex = 1;

		Ref<CommandBufferPoolVulkan> _pool;
		Ref<CommandBufferVulkan> _commandBuffer;
		Ref<GraphicsSemaphoreVulkan> _imageAvailableSemaphore;
		Ref<GraphicsSemaphoreVulkan> _renderingCompleteSemaphore;
		Ref<GraphicsFenceVulkan> _renderingCompleteFence;
		ManagedRef<RenderContextVulkanCache> _renderCache;

		RenderContextState _currentState;
		List<Ref<GraphicsSemaphoreVulkan>> _frameSignalSemaphores;
		List<Ref<GraphicsSemaphoreVulkan>> _frameWaitSemaphores;

		Ref<BufferVulkan> _globalUBO;
		VulkanDescriptorLayout _globalDescriptor;
		Ref<VulkanDescriptorPool> _globalDescriptorPool;
		VkDescriptorSet _globalDescriptorSet;

		VulkanRenderPass* _currentVulkanRenderPass = nullptr;
		VulkanFramebuffer* _currentFramebuffer = nullptr;

		Set<RenderContextStateChange> _stateChanges;
		ResourceID _currentShader = Resource::InvalidID;
		ResourceID _currentMaterial = Resource::InvalidID;
		VulkanPipeline* _currentPipeline = nullptr;
		UnorderedMap<ResourceID, VkDescriptorSet> _materialDescriptorSets;

		int _backbufferIndex = -1;

	public:
		RenderContextVulkan(ResourceID id, const string& name);
		~RenderContextVulkan() final;

		DefineResourceType(RenderContextVulkan)

		void SetViewport(const RectInt& rect) final;
		void UseShader(ResourceID shaderID) final;
		void UseMaterial(ResourceID materialID) final;
		void Draw(const ObjectRenderData& objectData) final;
		bool IsAvaliableForRendering() noexcept final { return _renderingCompleteFence->IsSignalled(); }
		void WaitForRenderingCompleted() final;

		/// @brief Gets the Vulkan semaphore that should be used to signal when the backbuffer is available
		/// @return The image available semaphore
		VkSemaphore GetImageAvailableSemaphore() noexcept { return _imageAvailableSemaphore->GetSemaphore(); }

		/// @brief Gets the Vulkan semaphore that is signaled when rendering has completed
		/// @return The render complete semaphore
		VkSemaphore GetRenderCompleteSemaphore() noexcept { return _renderingCompleteSemaphore->GetSemaphore(); }

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

		/// @brief Creates the global descriptor set
		void CreateGlobalDescriptorSet();

		/// @brief Creates a descriptor set for the currently bound material instance
		/// @param subshader The subshader to use
		/// @param materialID The ID of the material to use
		/// @param set Will be filled out with the descriptor set
		/// @return True if the descriptor set was created
		bool GetOrAllocateMaterialDescriptorSet(const VulkanShader& shader, const string& passName, ResourceID materialID, VkDescriptorSet& set);

		/// @brief Event handler for the device purging resources
		/// @return If the event was handled
		bool HandlePurgeResources();

		/// @brief Adds any necessary pre-render pass image transitions to the render targets
		void AddPreRenderPassImageTransitions();

		/// @brief Adds any necessary post-render pass image transitions to the render targets
		void AddPostRenderPassImageTransitions();
	};
}