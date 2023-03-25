#include "RenderContextVulkanCache.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Material.h>
#include "../GraphicsDeviceVulkan.h"
#include "../Cache/VulkanSubshader.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"
#include <Coco/Rendering/Graphics/RenderView.h>

namespace Coco::Rendering::Vulkan
{
	CachedSubshaderResource::CachedSubshaderResource(uint64_t key, const Ref<VulkanSubshader>& subshader) : 
		CachedResource(key, subshader->GetVersion()), SubshaderRef(subshader)
	{}

	bool CachedSubshaderResource::NeedsUpdate() const noexcept
	{
		if (Ref<VulkanSubshader> subshader = SubshaderRef.lock())
			return !Pool.IsValid() || subshader->GetVersion() != GetVersion();

		return false;
	}

	void CachedSubshaderResource::Update(GraphicsDeviceVulkan* device)
	{
		Ref<VulkanSubshader> subshader = SubshaderRef.lock();

		Pool = device->CreateResource<VulkanDescriptorPool>(MaxSets, List<VulkanDescriptorLayout>({ subshader->GetDescriptorLayout() }));
		UpdateVersion(subshader->GetVersion());
	}

	CachedMaterialResource::CachedMaterialResource(const MaterialRenderData& materialData) : CachedResource(materialData.ID, materialData.Version)
	{}

	bool CachedMaterialResource::NeedsUpdate(const MaterialRenderData& materialData) const noexcept
	{
		return BufferOffset == Math::MaxValue<uint64_t>() || BufferIndex == Math::MaxValue<uint>() || materialData.Version != GetVersion();
	}

	void CachedMaterialResource::Update(uint8_t* bufferMemory, const MaterialRenderData& materialData)
	{
		uint8_t* dst = bufferMemory + BufferOffset;
		std::memcpy(dst, materialData.UniformData.Data(), materialData.UniformData.Count());

		UpdateVersion(materialData.Version);
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
	{}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_materialCache.clear();
		_shaderCache.clear();
		_materialUBOs.Clear();
	}

	CachedSubshaderResource& RenderContextVulkanCache::GetSubshaderResource(const Ref<VulkanSubshader>& subshader)
	{
		std::hash<string> hasher;
		const uint64_t key = Math::CombineHashes(0, subshader->ShaderID, hasher(subshader->SubshaderName));

		if (!_shaderCache.contains(key))
			_shaderCache.emplace(key, CachedSubshaderResource(key, subshader));

		CachedSubshaderResource& resource = _shaderCache.at(key);
		Assert(resource.IsInvalid() == false);

		resource.UpdateTickUsed();

		if (resource.NeedsUpdate())
			resource.Update(_device);

		return resource;
	}

	CachedMaterialResource& RenderContextVulkanCache::GetMaterialResource(const MaterialRenderData& materialData)
	{
		if (!_materialCache.contains(materialData.ID))
			_materialCache.emplace(materialData.ID, CachedMaterialResource(materialData));

		CachedMaterialResource& resource = _materialCache.at(materialData.ID);

		resource.UpdateTickUsed();

		// Update the material data if needed
		if (resource.NeedsUpdate(materialData))
		{
			// Choose a new place in the buffers if this resource needs to expand
			if (materialData.UniformData.Count() > resource.BufferSize)
			{
				if (resource.BufferIndex != Math::MaxValue<uint>() && _materialUBOs.Count() > 0)
				{
					// The old region is now fragmented
					_materialUBOs[resource.BufferIndex].FragmentedSize += resource.BufferSize;
				}

				// Find a new buffer region
				resource.BufferSize = materialData.UniformData.Count();
				FindBufferRegion(resource.BufferSize, resource.BufferIndex, resource.BufferOffset);

				Assert(resource.BufferIndex != Math::MaxValue<uint>());
				Assert(resource.BufferOffset != Math::MaxValue<uint64_t>());
			}

			MaterialBuffer& buffer = _materialUBOs[resource.BufferIndex];

			// Map the buffer data if it isn't already
			if (buffer.MappedMemory == nullptr)
				buffer.MappedMemory = reinterpret_cast<uint8_t*>(buffer.Buffer->Lock(0, s_materialBufferSize));

			resource.Update(buffer.MappedMemory, materialData);
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

	WeakManagedRef<BufferVulkan> RenderContextVulkanCache::GetMaterialBuffer(uint bufferIndex) const noexcept
	{
		return _materialUBOs[bufferIndex].Buffer;
	}

	void RenderContextVulkanCache::PurgeResources() noexcept
	{
		// Purge shader cache
		uint64_t shadersPurged = 0;
		auto shaderIt = _shaderCache.begin();
		while (shaderIt != _shaderCache.end())
		{
			const CachedSubshaderResource& resource = (*shaderIt).second;

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
