#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include <Coco/Rendering/Material.h>
#include "VulkanIncludes.h"
#include "VulkanDescriptorPool.h"
#include "VulkanShader.h"
#include "BufferVulkan.h"
#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	/// <summary>
	/// A generic cached resource
	/// </summary>
	struct CachedResource
	{
		ResourceVersion Version;
		uint64_t LastTickUsed;

		CachedResource(ResourceVersion version);
		virtual ~CachedResource() = default;

		void UpdateTickUsed();
	};

	/// <summary>
	/// A cached shader resource
	/// </summary>
	struct CachedShaderResource : public CachedResource
	{
		const uint MaxSets = 512;
		GraphicsResourceRef<VulkanDescriptorPool> Pool = nullptr;

		CachedShaderResource(const VulkanShader* shader);

		bool NeedsUpdate(const VulkanShader* shader) const noexcept { return Pool == nullptr || shader->ShaderVersion != this->Version; }
		void Update(GraphicsDeviceVulkan* device, const VulkanShader* shader);
	};

	/// <summary>
	/// A cached material resource
	/// </summary>
	struct CachedMaterialResource : public CachedResource
	{
		uint64_t BufferOffset = Math::MaxValue<uint64_t>();
		uint64_t BufferSize = 0;

		CachedMaterialResource(const Material* material);

		bool NeedsUpdate(const Material* material) const { return BufferOffset == Math::MaxValue<uint64_t>() || material->GetVersion() != this->Version; }

		void Update(GraphicsDeviceVulkan* device, Material* material, void* bufferMemory, uint64_t currentBufferOffset, uint64_t bufferSize, bool& expanded);
	};

	/// <summary>
	/// A cache for a Vulkan render context that manages material data
	/// </summary>
	class RenderContextVulkanCache
	{
	private:
		GraphicsDeviceVulkan* _device;
		Map<ResourceID, CachedShaderResource> _shaderCache;
		Map<ResourceID, CachedMaterialResource> _materialCache;

	public:
		RenderContextVulkanCache(GraphicsDeviceVulkan* device);
		virtual ~RenderContextVulkanCache();

		/// <summary>
		/// Gets a shader resource, updating its last used tick
		/// </summary>
		/// <param name="shader">The shader that the resource points to</param>
		/// <returns>The cached resource for the shader</returns>
		CachedShaderResource& GetShaderResource(const VulkanShader* shader);

		/// <summary>
		/// Gets a material resource, updating its last used tick
		/// </summary>
		/// <param name="material">The material that the resource points to</param>
		/// <returns>The cached resource for the material</returns>
		CachedMaterialResource& GetMaterialResource(const Material* material);

		/// <summary>
		/// Frees all descriptor sets from cached shader pools
		/// </summary>
		void FreeDescriptorSets();
	};
}