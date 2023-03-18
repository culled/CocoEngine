#include "RenderContextVulkanCache.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::Rendering::Vulkan
{
	CachedResource::CachedResource(ResourceVersion version) :
		Version(version)
	{
		UpdateTickUsed();
	}

	void CachedResource::UpdateTickUsed()
	{
		LastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}

	CachedShaderResource::CachedShaderResource(const VulkanShader* shader) : CachedResource(shader->ShaderVersion)
	{}

	void CachedShaderResource::Update(GraphicsDeviceVulkan* device, const VulkanShader* shader)
	{
		Pool = device->CreateResource<VulkanDescriptorPool>(MaxSets, shader->GetDescriptorLayouts());
		this->Version = shader->ShaderVersion;
	}

	CachedMaterialResource::CachedMaterialResource(const Material* material) : CachedResource(material->GetVersion())
	{}

	void CachedMaterialResource::Update(GraphicsDeviceVulkan* device, Material* material, void* bufferMemory, uint64_t currentBufferOffset, uint64_t bufferSize, bool& appended)
	{
		const List<uint8_t>& materialData = material->GetBufferData();

		appended = BufferSize < materialData.Count();
		BufferSize = materialData.Count();

		if (BufferSize == 0)
			return;

		// TODO: gracefully handle out of buffer memory
		if (currentBufferOffset + BufferSize > bufferSize)
			throw VulkanRenderingException("Out of material buffer memory");

		if (BufferOffset > currentBufferOffset || appended)
			BufferOffset = currentBufferOffset;

		uint8_t* dst = reinterpret_cast<uint8_t*>(bufferMemory) + BufferOffset;
		std::memcpy(dst, materialData.Data(), BufferSize);

		this->Version = material->GetVersion();
	}

	RenderContextVulkanCache::RenderContextVulkanCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_materialCache.clear();
		_shaderCache.clear();
	}

	CachedShaderResource& RenderContextVulkanCache::GetShaderResource(const VulkanShader* shader)
	{
		if (!_shaderCache.contains(shader->ShaderID))
			_shaderCache.emplace(shader->ShaderID, CachedShaderResource(shader));

		CachedShaderResource& resource = _shaderCache.at(shader->ShaderID);
		resource.UpdateTickUsed();

		return resource;
	}

	CachedMaterialResource& RenderContextVulkanCache::GetMaterialResource(const Material* material)
	{
		if (!_materialCache.contains(material->GetID()))
			_materialCache.emplace(material->GetID(), CachedMaterialResource(material));

		CachedMaterialResource& resource = _materialCache.at(material->GetID());
		resource.UpdateTickUsed();

		return resource;
	}

	void RenderContextVulkanCache::FreeDescriptorSets()
	{
		for (auto& shaderKVP : _shaderCache)
			shaderKVP.second.Pool->FreeSets();
	}
}
