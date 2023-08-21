#pragma once

#include <Coco/Core/Resources/ResourceCache.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "Resources/Cache/VulkanFramebuffer.h"
#include "Resources/Cache/VulkanShaderResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	struct MaterialRenderData;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanShader;
	class VulkanRenderPass;
	class BufferVulkan;
	class VulkanDescriptorPool;

	/// @brief A cache for a Vulkan render context that manages material data
	class RenderContextVulkanCache : public BaseGraphicsResource<GraphicsDeviceVulkan>
	{
	private:
		ManagedRef<ResourceCache<VulkanShaderResource>> _shaderCache;
		ManagedRef<ResourceCache<VulkanFramebuffer>> _framebufferCache;

	public:
		RenderContextVulkanCache();
		virtual ~RenderContextVulkanCache();

		/// @brief Gets a shader resource
		/// @param shader The shader that the resource points to
		/// @return The cached resource for the shader
		VulkanShaderResource* GetOrCreateShaderResource(const VulkanShader& shader);

		/// @brief Gets or creates a framebuffer resource
		/// @param renderView The renderview
		/// @param renderPass The render pass
		/// @param pipeline The render pipeline
		/// @return The cached resource for the framebuffer
		VulkanFramebuffer* GetOrCreateFramebuffer(Ref<RenderView>& renderView, VulkanRenderPass& renderPass, Ref<RenderPipeline> pipeline);

		/// @brief Flushes any pending changes to the shader uniform buffers
		void FlushUniformBufferChanges();

		/// @brief Frees all descriptor sets from cached shader pools
		void FreeDescriptorSets();

		/// @brief Purges unused and invalid resources 
		void PurgeResources() noexcept;
	};
}