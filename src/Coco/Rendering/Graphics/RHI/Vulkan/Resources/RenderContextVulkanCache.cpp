#include "RenderContextVulkanCache.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::Rendering::Vulkan
{
	CachedShaderResource::CachedShaderResource(Ref<VulkanShader> shader) : CachedResource(shader->ID, shader->Version), ShaderRef(shader)
	{}

	bool CachedShaderResource::NeedsUpdate() const noexcept
	{
		if (Ref<VulkanShader> shader = ShaderRef.lock())
			return !Pool.IsValid() || shader->Version != this->Version;

		return false;
	}

	void CachedShaderResource::Update(GraphicsDeviceVulkan* device)
	{
		Ref<VulkanShader> shader = ShaderRef.lock();

		Pool = device->CreateResource<VulkanDescriptorPool>(MaxSets, shader->GetDescriptorLayouts());
		this->Version = shader->Version;
	}

	CachedMaterialResource::CachedMaterialResource(Ref<Material> material) : CachedResource(material->GetID(), material->GetVersion()), MaterialRef(material)
	{}

	bool CachedMaterialResource::NeedsUpdate() const noexcept
	{
		if (Ref<Material> mat = MaterialRef.lock())
			return BufferOffset == Math::MaxValue<uint64_t>() || mat->GetVersion() != this->Version;

		return false;
	}

	void CachedMaterialResource::Update(uint8_t* bufferMemory, const uint8_t* materialData, uint64_t materialDataLength)
	{
		uint8_t* dst = bufferMemory + BufferOffset;
		std::memcpy(dst, materialData, materialDataLength);

		this->Version = MaterialRef.lock()->GetVersion();
	}

	void CachedMaterialResource::InvalidateBufferRegion()
	{
		BufferOffset = Math::MaxValue<uint64_t>();
		BufferSize = 0;
	}

	RenderContextVulkanCache::RenderContextVulkanCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{
		_materialUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			s_materialBufferSize,
			true);
	}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_materialCache.clear();
		_shaderCache.clear();
		_materialUBO.Invalidate();
	}

	CachedShaderResource& RenderContextVulkanCache::GetShaderResource(Ref<VulkanShader> shader)
	{
		if (!_shaderCache.contains(shader->ID))
			_shaderCache.emplace(shader->ID, CachedShaderResource(shader));

		CachedShaderResource& resource = _shaderCache.at(shader->ID);
		Assert(resource.IsInvalid() == false);

		resource.UpdateTickUsed();

		if (resource.NeedsUpdate())
			resource.Update(_device);

		return resource;
	}

	CachedMaterialResource& RenderContextVulkanCache::GetMaterialResource(Ref<Material> material)
	{
		if (!_materialCache.contains(material->GetID()))
			_materialCache.emplace(material->GetID(), CachedMaterialResource(material));

		CachedMaterialResource& resource = _materialCache.at(material->GetID());
		Assert(resource.IsInvalid() == false);

		resource.UpdateTickUsed();

		// Update the material data if needed
		if (resource.NeedsUpdate())
		{
			const List<uint8_t>& data = material->GetBufferData();

			// Choose a place in the buffer if this resource needs to expand
			if (data.Count() > resource.BufferSize)
			{
				if (data.Count() > _materialUBO->GetSize() - _currentMaterialUBOOffset)
				{
					// TODO: make new buffer if out of memory?
					InvalidateMaterialBufferRegions(0);
					throw VulkanRenderingException("Out of material memory");
				}

				resource.BufferOffset = _currentMaterialUBOOffset;
				resource.BufferSize = data.Count();
				_currentMaterialUBOOffset += resource.BufferSize;

				_averageMaterialDataSize += static_cast<double>(resource.BufferSize - _averageMaterialDataSize) / _materialCache.size();
			}

			// Map the buffer data if it isn't already
			if (_mappedMaterialUBOMemory == nullptr)
				_mappedMaterialUBOMemory = reinterpret_cast<uint8_t*>(_materialUBO->Lock(0, s_materialBufferSize));

			resource.Update(_mappedMaterialUBOMemory, data.Data(), data.Count());
		}

		return resource;
	}

	void RenderContextVulkanCache::FlushMaterialChanges()
	{
		if (_mappedMaterialUBOMemory != nullptr)
		{
			_materialUBO->Unlock();
			_mappedMaterialUBOMemory = nullptr;
		}

		// TEMPORARY HACK: We'll need a better way of managing the buffer data in the future, but for now this should defragment it if its getting too large
		if (_materialUBO->GetSize() - _currentMaterialUBOOffset < _averageMaterialDataSize * 2.0)
		{
			LogInfo(_device->GetLogger(), FormattedString(
				"Material buffer has used {}/{} bytes (Average data size is {} bytes). Invalidating regions for next frame",
				_currentMaterialUBOOffset,
				_materialUBO->GetSize(), _averageMaterialDataSize
			));

			InvalidateMaterialBufferRegions(0);
		}
	}

	void RenderContextVulkanCache::FreeDescriptorSets()
	{
		for (auto& shaderKVP : _shaderCache)
			shaderKVP.second.Pool->FreeSets();
	}

	void RenderContextVulkanCache::PurgeResources() noexcept
	{
		// Purge shader cache
		uint64_t shadersPurged = 0;
		auto shaderIt = _shaderCache.begin();
		while (shaderIt != _shaderCache.end())
		{
			const CachedShaderResource& resource = (*shaderIt).second;

			if (resource.ShouldPurge(s_staleTickCount))
			{
				shaderIt = _shaderCache.erase(shaderIt);
				shadersPurged++;
			}
			else
				shaderIt++;
		}

		// Purge material cache
		uint64_t materialsPurged = 0;
		auto materialIt = _materialCache.begin();
		while (materialIt != _materialCache.end())
		{
			const CachedMaterialResource& resource = (*materialIt).second;

			if (resource.ShouldPurge(s_staleTickCount))
			{
				materialIt = _materialCache.erase(materialIt);
				materialsPurged++;
			}
			else
				materialIt++;
		}

		if (shadersPurged > 0 || materialsPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString("Purged {} cached shaders and {} cached materials from RenderContext cache", shadersPurged, materialsPurged));
	}

	void RenderContextVulkanCache::InvalidateMaterialBufferRegions(uint64_t invalidStartOffset)
	{
		for (auto& materialKVP : _materialCache)
		{
			CachedMaterialResource& resource = materialKVP.second;
			if (resource.BufferOffset >= invalidStartOffset || resource.BufferOffset + resource.BufferSize > invalidStartOffset)
			{
				_currentMaterialUBOOffset = Math::Min(_currentMaterialUBOOffset, resource.BufferOffset);
				resource.InvalidateBufferRegion();
			}
		}
	}
}
