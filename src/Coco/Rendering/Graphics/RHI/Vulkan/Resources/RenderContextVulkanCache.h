#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Material.h>
#include "../VulkanIncludes.h"
#include "../GraphicsDeviceVulkan.h"
#include "../VulkanShader.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"

namespace Coco::Rendering::Vulkan
{
	/// <summary>
	/// A cached shader resource
	/// </summary>
	struct CachedShaderResource : public CachedResource
	{
		const uint MaxSets = 512;
		WeakManagedRef<VulkanDescriptorPool> Pool;
		WeakRef<VulkanShader> ShaderRef;

		CachedShaderResource(Ref<VulkanShader> shader);

		bool IsInvalid() const noexcept final { return ShaderRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// <summary>
		/// Updates this cached resource
		/// </summary>
		/// <param name="device">The device</param>
		void Update(GraphicsDeviceVulkan* device);
	};

	/// <summary>
	/// A cached material resource
	/// </summary>
	struct CachedMaterialResource final : public CachedResource
	{
		uint64_t BufferOffset = Math::MaxValue<uint64_t>();
		uint64_t BufferSize = 0;
		WeakRef<Material> MaterialRef;

		CachedMaterialResource(Ref<Material> material);

		bool IsInvalid() const noexcept final { return MaterialRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// <summary>
		/// Updates this cached material's region of data in the material buffer
		/// </summary>
		/// <param name="bufferMemory">The memory of the buffer</param>
		/// <param name="materialData">The material data</param>
		/// <param name="materialDataLength">The size of the material data</param>
		void Update(uint8_t* bufferMemory, const uint8_t* materialData, uint64_t materialDataLength);

		/// <summary>
		/// Invalidates this cached material's region of data
		/// </summary>
		void InvalidateBufferRegion();
	};

	/// <summary>
	/// A cache for a Vulkan render context that manages material data
	/// </summary>
	class RenderContextVulkanCache
	{
	private:
		static constexpr uint64_t s_materialBufferSize = 1024 * 1024 * 10; // 10 MiB
		static constexpr uint64_t s_staleTickCount = 500;

		GraphicsDeviceVulkan* _device;
		Map<ResourceID, CachedShaderResource> _shaderCache;
		Map<ResourceID, CachedMaterialResource> _materialCache;

		WeakManagedRef<BufferVulkan> _materialUBO;
		uint64_t _currentMaterialUBOOffset = 0;
		uint8_t* _mappedMaterialUBOMemory = nullptr;
		double _averageMaterialDataSize = 0.0;

	public:
		RenderContextVulkanCache(GraphicsDeviceVulkan* device);
		virtual ~RenderContextVulkanCache();

		/// <summary>
		/// Gets a shader resource, updating its last used tick
		/// </summary>
		/// <param name="shader">The shader that the resource points to</param>
		/// <returns>The cached resource for the shader</returns>
		CachedShaderResource& GetShaderResource(Ref<VulkanShader> shader);

		/// <summary>
		/// Gets a material resource, updating its last used tick
		/// </summary>
		/// <param name="material">The material that the resource points to</param>
		/// <returns>The cached resource for the material</returns>
		CachedMaterialResource& GetMaterialResource(Ref<Material> material);

		/// <summary>
		/// Flushes any pending material changes to the material buffers
		/// </summary>
		void FlushMaterialChanges();

		/// <summary>
		/// Frees all descriptor sets from cached shader pools
		/// </summary>
		void FreeDescriptorSets();

		/// <summary>
		/// Gets the material buffer
		/// </summary>
		/// <returns>The material buffer</returns>
		WeakManagedRef<BufferVulkan> GetMaterialBuffer() const noexcept { return _materialUBO; }

		/// <summary>
		/// Purges unused and invalid resources
		/// </summary>
		void PurgeResources() noexcept;

	private:
		/// <summary>
		/// Invalidates all material regions that overlap or begin past the given offset
		/// </summary>
		/// <param name="invalidStartOffset">The offset to start invalidating regions</param>
		void InvalidateMaterialBufferRegions(uint64_t invalidStartOffset);
	};
}