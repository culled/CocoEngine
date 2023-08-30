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
		friend class ManagedRef<RenderContextVulkan>;

	private:
		static constexpr uint _globalDescriptorSetIndex = 0;
		static constexpr uint _instanceDescriptorSetIndex = 1;

		Ref<CommandBufferPoolVulkan> _pool;
		Ref<CommandBufferVulkan> _commandBuffer;
		Ref<GraphicsSemaphoreVulkan> _imageAvailableSemaphore;
		Ref<GraphicsSemaphoreVulkan> _renderingCompleteSemaphore;
		Ref<GraphicsFenceVulkan> _renderingCompleteFence;
		ManagedRef<RenderContextVulkanCache> _renderCache;

		RenderContextState _currentState;
		List<Ref<GraphicsSemaphoreVulkan>> _frameSignalSemaphores;
		List<Ref<GraphicsSemaphoreVulkan>> _frameWaitSemaphores;

		VulkanRenderPass* _currentVulkanRenderPass = nullptr;
		VulkanFramebuffer* _currentFramebuffer = nullptr;

		Set<RenderContextStateChange> _stateChanges;
		VulkanPipeline* _currentPipeline = nullptr;

		int _backbufferIndex = -1;

	protected:
		RenderContextVulkan(const ResourceID& id, const string& name);

	public:
		~RenderContextVulkan() final;

		DefineResourceType(RenderContextVulkan)

		void SetViewport(const RectInt& rect) final;
		void Draw(const ObjectRenderData& objectData, uint submeshIndex) final;
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
		bool BeginImpl() final;
		bool BeginPassImpl() final;
		void EndImpl() final;
		void ResetImpl() final;
		void NewShaderBound() final;
		void NewMaterialBound() final;
		void UniformUpdated(const string& name) final;
		void TextureSamplerUpdated(const string& name) final;

	private:
		/// @brief Attempts to flush all state changes and bind the current state
		/// @return True if the state is bound
		bool FlushStateChanges();

		/// @brief Applies the current push constants
		void ApplyPushConstants();

		/// @brief Adds any necessary pre-render pass image transitions to the render targets
		void AddPreRenderPassImageTransitions();

		/// @brief Adds any necessary post-render pass image transitions to the render targets
		void AddPostRenderPassImageTransitions();
	};
}