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
			return BufferOffset == Math::MaxValue<uint64_t>() || BufferIndex == Math::MaxValue<uint>() || mat->GetVersion() != this->Version;

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

	uint64_t MaterialBuffer::GetFreeRegionSize() const noexcept
	{
		return Buffer->GetSize() - CurrentOffset;
	}

	RenderContextVulkanCache::RenderContextVulkanCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{
		//CreateAdditionalMaterialBuffer();
	}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_materialCache.clear();
		_shaderCache.clear();
		_materialUBOs.Clear();
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

			// Choose a new place in the buffers if this resource needs to expand
			if (data.Count() > resource.BufferSize)
			{
				if (resource.BufferIndex != Math::MaxValue<uint>() && _materialUBOs.Count() > 0)
				{
					// The old region is now fragmented
					_materialUBOs[resource.BufferIndex].FragmentedSize += resource.BufferSize;
				}

				// Find a new buffer region
				resource.BufferSize = data.Count();
				FindBufferRegion(resource.BufferSize, resource.BufferIndex, resource.BufferOffset);

				Assert(resource.BufferIndex != Math::MaxValue<uint>());
				Assert(resource.BufferOffset != Math::MaxValue<uint64_t>());
			}

			MaterialBuffer& buffer = _materialUBOs[resource.BufferIndex];

			// Map the buffer data if it isn't already
			if (buffer.MappedMemory == nullptr)
				buffer.MappedMemory = reinterpret_cast<uint8_t*>(buffer.Buffer->Lock(0, s_materialBufferSize));

			resource.Update(buffer.MappedMemory, data.Data(), data.Count());
		}

		return resource;
	}

	void RenderContextVulkanCache::FlushMaterialChanges()
	{
		for (int i = 0; i < _materialUBOs.Count(); i++)
		{
			MaterialBuffer& buffer = _materialUBOs[i];

			// Flush the buffer if it's mapped
			if(buffer.MappedMemory != nullptr)
				buffer.Buffer->Unlock();

			buffer.MappedMemory = nullptr;

			// If the buffer is too fragmented, invalidate it to cause a reshuffling of data
			if (static_cast<double>(buffer.FragmentedSize) / buffer.Buffer->GetSize() > s_fragmentFlushThreshold)
			{
				LogInfo(_device->GetLogger(), FormattedString("Material buffer {} is too fragmented. Invalidating for new frame...", i));
				InvalidateMaterialBufferRegions(i);
			}
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

	void RenderContextVulkanCache::InvalidateMaterialBufferRegions(uint bufferIndex)
	{
		MaterialBuffer& buffer = _materialUBOs[bufferIndex];

		for (auto& materialKVP : _materialCache)
		{
			CachedMaterialResource& resource = materialKVP.second;
			if (resource.BufferIndex != bufferIndex)
				continue;

			resource.InvalidateBufferRegion();
		}

		buffer.CurrentOffset = 0;
		buffer.FragmentedSize = 0;
	}

	void RenderContextVulkanCache::FindBufferRegion(uint64_t requiredSize, uint& bufferIndex, uint64_t& bufferOffset)
	{
		int freeBufferIndex = -1;

		// Try to find a buffer that can fit this material data
		for (int i = 0; i < _materialUBOs.Count(); i++)
		{
			if (_materialUBOs[i].GetFreeRegionSize() >= requiredSize)
			{
				freeBufferIndex = i;
				break;
			}
		}

		// Make a new buffer if we can't fit the material in the current one
		if (freeBufferIndex == -1)
		{
			CreateAdditionalMaterialBuffer();
			LogInfo(_device->GetLogger(), FormattedString("Created addition material buffer. New buffer count is {}", _materialUBOs.Count()));
			freeBufferIndex = static_cast<int>(_materialUBOs.Count()) - 1;
		}

		Assert(freeBufferIndex != -1);

		bufferIndex = static_cast<uint>(freeBufferIndex);
		bufferOffset = _materialUBOs[freeBufferIndex].CurrentOffset;

		_materialUBOs[freeBufferIndex].CurrentOffset += requiredSize;
	}

	void RenderContextVulkanCache::CreateAdditionalMaterialBuffer()
	{
		MaterialBuffer buffer = {};
		buffer.Buffer = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			s_materialBufferSize,
			true);

		_materialUBOs.Add(buffer);
	}
}
