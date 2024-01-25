#include "Renderpch.h"
#include "VulkanUniformCache.h"

#include "VulkanUniformCache.h"
#include "VulkanGraphicsDevice.h"
#include "../../../Texture.h"

#include <Coco/Core/Engine.h>
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	CachedVulkanUniformData::CachedVulkanUniformData(uint64 dataID, UniformScope scope, Ref<VulkanBuffer> buffer) :
		DataID(dataID),
		Scope(scope),
		Buffer(buffer),
		AllocBlock(),
		Textures(),
		LastUsageTime(0.0)
	{}

	void CachedVulkanUniformData::Use()
	{
		LastUsageTime = MainLoop::Get()->GetCurrentTick().Time;
	}

	VulkanUniformCache::VulkanUniformCache(VulkanGraphicsDevice& device) :
		_device(device),
		_globalCache(),
		_instanceCache(),
		_globalUniformBuffer(),
		_instanceUniformBuffer()
	{
		// TODO: temporary!
		const uint64 bufferSize = 1024 * 10;

		_globalUniformBuffer = StaticRefCast<VulkanBuffer>(_device.CreateBuffer(
			bufferSize,
			BufferUsageFlags::HostVisible | BufferUsageFlags::Uniform));

		_instanceUniformBuffer = StaticRefCast<VulkanBuffer>(_device.CreateBuffer(
			bufferSize,
			BufferUsageFlags::HostVisible | BufferUsageFlags::Uniform));
	}

	VulkanUniformCache::~VulkanUniformCache()
	{
		_globalCache.clear();
		_instanceCache.clear();

		_device.TryReleaseResource(_globalUniformBuffer->ID);
		_device.TryReleaseResource(_instanceUniformBuffer->ID);
	}

	CachedVulkanUniformData& VulkanUniformCache::CreateOrGetData(uint64 dataID, UniformScope dataScope, const ShaderUniformLayout& layout)
	{
		std::unordered_map<uint64, CachedVulkanUniformData>* cache = GetCacheForScope(dataScope);
		auto it = cache->find(dataID);

		if (it != cache->end())
			return it->second;

		std::vector<ShaderUniformValue> uniforms = layout.GetDefaultValues();

		return CreateOrUpdateData(dataID, 0, dataScope, layout, uniforms);
	}

	CachedVulkanUniformData& VulkanUniformCache::CreateOrUpdateData(
		uint64 dataID, 
		uint64 dataVersion, 
		UniformScope dataScope, 
		const ShaderUniformLayout& layout, 
		std::span<const ShaderUniformValue> values)
	{
		CocoAssert(!layout.NeedsDataCalculation(), "Layout data uniforms have not been calculated")

		std::unordered_map<uint64, CachedVulkanUniformData>* cache = GetCacheForScope(dataScope);
		Ref<VulkanBuffer> buffer = GetBufferForScope(dataScope);

		CocoAssert(cache, "Unsupported cached data scope")
		CocoAssert(buffer, "Unsupported cached data scope")

		CachedVulkanUniformData& cachedData = cache->try_emplace(dataID, dataID, dataScope, buffer).first->second;

		// TODO: don't update uniform data if it's the same version

		try
		{
			std::vector<uint8> data = layout.GetBufferData(values);
			EnsureCachedDataIsAllocated(data.size(), cachedData);

			if (data.size() > 0)
			{
				// Copy uniform data
				char* bufferMemory = (char*)buffer->GetMappedData();
				Assert(memcpy_s(bufferMemory + cachedData.AllocBlock->Offset, cachedData.AllocBlock->Size, data.data(), data.size()) == 0)
			}

			auto textures = layout.GetTextures(values);
			cachedData.Textures.clear();
			for (const auto& kvp : textures)
			{
				cachedData.Textures.try_emplace(kvp.first, kvp.second);
			}

			cachedData.DataVersion = dataVersion;
			cachedData.Use();
		}
		catch (const Exception& ex)
		{
			FreeCachedUniformData(cachedData);

			CocoError("Update CachedVulkanUniform error: {}", ex.what())
		}

		return cachedData;
	}

	bool VulkanUniformCache::TryGetCachedData(uint64 dataID, UniformScope dataScope, const CachedVulkanUniformData*& outData)
	{
		std::unordered_map<uint64, CachedVulkanUniformData>* cache = GetCacheForScope(dataScope);
		CocoAssert(cache, "Unsupported cached data scope")

		auto it = cache->find(dataID);
		if (it == cache->end())
			return false;

		CachedVulkanUniformData& data = it->second;
		data.Use();

		outData = &data;
		return true;
	}

	bool VulkanUniformCache::DataNeedsUpdate(uint64 dataID, UniformScope dataScope, uint64 dataVersion) const
	{
		const std::unordered_map<uint64, CachedVulkanUniformData>* cache = GetCacheForScope(dataScope);
		
		if (!cache)
			return false;

		auto it = cache->find(dataID);
		if (it == cache->end())
			return true;

		return it->second.DataVersion != dataVersion;
	}

	uint64 VulkanUniformCache::PurgeUnusedUniformData(double staleThreshold)
	{
		uint64 purgedData = PurgeUniformCache(UniformScope::Global, staleThreshold);
		purgedData += PurgeUniformCache(UniformScope::Instance, staleThreshold);

		return purgedData;
	}

	const std::unordered_map<uint64, CachedVulkanUniformData>* VulkanUniformCache::GetCacheForScope(UniformScope dataScope) const
	{
		switch (dataScope)
		{
		case UniformScope::Global:
		case UniformScope::ShaderGlobal:
			return &_globalCache;
		case UniformScope::Instance:
			return &_instanceCache;
		default:
			CocoAssert(false, "Unsupported cached data scope")
				return nullptr;
		}
	}

	std::unordered_map<uint64, CachedVulkanUniformData>* VulkanUniformCache::GetCacheForScope(UniformScope dataScope)
	{
		switch (dataScope)
		{
		case UniformScope::Global:
		case UniformScope::ShaderGlobal:
			return &_globalCache;
		case UniformScope::Instance:
			return &_instanceCache;
		default:
			CocoAssert(false, "Unsupported cached data scope")
			return nullptr;
		}
	}

	Ref<VulkanBuffer> VulkanUniformCache::GetBufferForScope(UniformScope dataScope)
	{
		switch (dataScope)
		{
		case UniformScope::Global:
		case UniformScope::ShaderGlobal:
			return _globalUniformBuffer;
		case UniformScope::Instance:
			return _instanceUniformBuffer;
		default:
			CocoAssert(false, "Unsupported cached data scope")
				return nullptr;
		}
	}

	void VulkanUniformCache::EnsureCachedDataIsAllocated(uint64 dataSize, CachedVulkanUniformData& cachedData)
	{
		// Free the space if it isn't the same as required
		if (cachedData.AllocBlock.has_value() && dataSize != cachedData.AllocBlock->Size)
			FreeCachedUniformData(cachedData);

		// Allocate space for the uniform data, if needed
		if (!cachedData.AllocBlock.has_value() && dataSize > 0)
		{
			CocoAssert(cachedData.Buffer.IsValid(), "Data's buffer was invalid")

			if (!cachedData.Buffer->VirtualAllocate(dataSize, cachedData.AllocBlock.emplace()))
			{
				throw Exception(FormatString("Could not allocate {} bytes for uniform data", dataSize));
			}
		}
	}

	void VulkanUniformCache::FreeCachedUniformData(CachedVulkanUniformData& cachedData)
	{
		if (!cachedData.AllocBlock.has_value())
			return;

		// Free the uniform data from the buffer if it exists
		cachedData.Buffer->VirtualFree(cachedData.AllocBlock.value());
		cachedData.AllocBlock.reset();
	}

	uint64 VulkanUniformCache::PurgeUniformCache(UniformScope dataScope, double staleThreshold)
	{
		double time = MainLoop::cGet()->GetCurrentTick().Time;
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		std::unordered_map<uint64, CachedVulkanUniformData>* cache = GetCacheForScope(dataScope);

		uint64 purgedData = 0;

		auto it = cache->begin();
		while (it != cache->end())
		{
			CachedVulkanUniformData& data = it->second;

			if (time - data.LastUsageTime > staleThreshold)
			{
				FreeCachedUniformData(data);
				it = cache->erase(it);
				purgedData++;
			}
			else
			{
				++it;
			}
		}

		return purgedData;
	}
}