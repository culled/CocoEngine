#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	class Material;
}

namespace Coco::Rendering::Vulkan
{
	class VulkanShader;
	class GraphicsDeviceVulkan;
	class BufferVulkan;
	class VulkanDescriptorPool;

	/// @brief A cached shader resource
	struct CachedShaderResource : public CachedResource
	{
		const uint MaxSets = 512;
		WeakManagedRef<VulkanDescriptorPool> Pool;
		WeakRef<VulkanShader> ShaderRef;

		CachedShaderResource(const Ref<VulkanShader>& shader);

		bool IsInvalid() const noexcept final { return ShaderRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Updates this cached resource
		/// @param device The device
		void Update(GraphicsDeviceVulkan* device);
	};

	/// @brief A cached material resource
	struct CachedMaterialResource final : public CachedResource
	{
		uint BufferIndex = Math::MaxValue<uint>();
		uint64_t BufferOffset = Math::MaxValue<uint64_t>();
		uint64_t BufferSize = 0;
		WeakRef<Material> MaterialRef;

		CachedMaterialResource(const Ref<Material>& material);

		bool IsInvalid() const noexcept final { return MaterialRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Updates this cached material's region of data in the material buffer
		/// @param bufferMemory The memory of the buffer
		/// @param materialData The material data
		/// @param materialDataLength The size of the material data
		void Update(uint8_t* bufferMemory, const uint8_t* materialData, uint64_t materialDataLength);

		/// @brief Invalidates this cached material's region of data
		void InvalidateBufferRegion();
	};

	/// @brief Holds material property data
	struct MaterialBuffer
	{
		/// @brief The buffer
		WeakManagedRef<BufferVulkan> Buffer;

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
	class RenderContextVulkanCache
	{
	private:
		static constexpr uint64_t s_materialBufferSize = 1024 * 1024 * 10; // 10 MiB
		static constexpr uint64_t s_staleTickCount = 500;
		static constexpr double s_fragmentFlushThreshold = 0.5;

		GraphicsDeviceVulkan* _device;
		Map<ResourceID, CachedShaderResource> _shaderCache;
		Map<ResourceID, CachedMaterialResource> _materialCache;

		List<MaterialBuffer> _materialUBOs;

	public:
		RenderContextVulkanCache(GraphicsDeviceVulkan* device);
		virtual ~RenderContextVulkanCache();

		/// @brief Gets a shader resource, updating its last used tick
		/// @param shader The shader that the resource points to
		/// @return The cached resource for the shader
		CachedShaderResource& GetShaderResource(const Ref<VulkanShader>& shader);

		/// @brief Gets a material resource, updating its last used tick
		/// @param material The material that the resource points to
		/// @return The cached resource for the material
		CachedMaterialResource& GetMaterialResource(const Ref<Material>& material);

		/// @brief Flushes any pending material changes to the material buffers
		void FlushMaterialChanges();

		/// @brief Frees all descriptor sets from cached shader pools
		void FreeDescriptorSets();

		/// @brief Gets the material buffer
		/// @param bufferIndex The index of the material buffer
		/// @return The material buffer
		WeakManagedRef<BufferVulkan> GetMaterialBuffer(uint bufferIndex) const noexcept;

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
		void FindBufferRegion(uint64_t requiredSize, uint& bufferIndex, uint64_t& bufferOffset);

		/// @brief Creates an additional material buffer
		void CreateAdditionalMaterialBuffer();
	};
}