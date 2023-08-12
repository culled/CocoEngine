#pragma once

#include <Coco/Core/Resources/ResourceCache.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "Resources/Cache/VulkanFramebuffer.h"
#include "Resources/Cache/VulkanMaterialResource.h"
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

	/// @brief Holds material property data
	struct MaterialBuffer
	{
		/// @brief The buffer
		Ref<BufferVulkan> Buffer;

		/// @brief The next open offset in the buffer
		uint64_t CurrentOffset = 0;
		
		/// @brief The amount of fragmented memory behind the current offset
		uint64_t FragmentedSize = 0;

		/// @brief The mapped memory of the buffer (if it has been locked)
		uint8_t* MappedMemory = nullptr;

		/// @brief Gets the size of the available memory region
		/// @return The size of the free space past the current offset in the buffer
		uint64_t GetFreeRegionSize() const noexcept;
	};

	/// @brief A cache for a Vulkan render context that manages material data
	class RenderContextVulkanCache : public BaseGraphicsResource<GraphicsDeviceVulkan>
	{
	private:
		static constexpr uint64_t _materialBufferSize = 1024 * 1024 * 10; // 10 MiB
		static constexpr double _fragmentFlushThreshold = 0.5;

		ManagedRef<ResourceCache<VulkanShaderResource>> _shaderCache;
		ManagedRef<ResourceCache<VulkanMaterialResource>> _materialCache;
		ManagedRef<ResourceCache<VulkanFramebuffer>> _framebufferCache;

		List<MaterialBuffer> _materialUBOs;

	public:
		RenderContextVulkanCache();
		virtual ~RenderContextVulkanCache();

		/// @brief Gets a shader resource
		/// @param shader The shader that the resource points to
		/// @return The cached resource for the shader
		VulkanShaderResource* GetOrCreateShaderResource(const VulkanShader& shader);

		/// @brief Gets a material resource
		/// @param materialData The material data that the resource points to
		/// @return The cached resource for the material data
		VulkanMaterialResource* GetOrCreateMaterialResource(const MaterialRenderData& materialData);

		/// @brief Gets or creates a framebuffer resource
		/// @param renderView The renderview
		/// @param renderPass The render pass
		/// @param pipeline The render pipeline
		/// @return The cached resource for the framebuffer
		VulkanFramebuffer* GetOrCreateFramebuffer(Ref<RenderView>& renderView, VulkanRenderPass& renderPass, Ref<RenderPipeline> pipeline);

		/// @brief Flushes any pending material changes to the material buffers
		void FlushMaterialChanges();

		/// @brief Frees all descriptor sets from cached shader pools
		void FreeDescriptorSets();

		/// @brief Gets the material buffer
		/// @param bufferIndex The index of the material buffer
		/// @return The material buffer
		Ref<BufferVulkan> GetMaterialBuffer(uint bufferIndex) const noexcept;

		/// @brief Purges unused and invalid resources 
		void PurgeResources() noexcept;

	private:
		/// @brief Invalidates all cached material regions in a material buffer
		/// @param bufferIndex The index of the material buffer
		void InvalidateMaterialBufferRegions(uint bufferIndex);

		/// @brief Finds a region in a material buffer to fit material data
		/// @param requiredSize The required size of the region
		/// @param bufferIndex Will be set to the index of the buffer to use
		/// @param bufferOffset Will be set to the offset in the buffer
		void FindBufferRegion(VulkanMaterialResource* resource, const MaterialRenderData& materialData);

		/// @brief Creates an additional material buffer
		void CreateAdditionalMaterialBuffer();
	};
}