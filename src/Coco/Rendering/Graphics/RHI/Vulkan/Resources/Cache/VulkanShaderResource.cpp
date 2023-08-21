#include "VulkanShaderResource.h"

#include "VulkanShader.h"
#include "../VulkanDescriptorPool.h"
#include "../../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	uint64_t ShaderUniformBuffer::GetFreeRegionSize() const noexcept
	{
		return Buffer->GetSize() - CurrentOffset;
	}

	void ShaderUniformBufferRegion::Invalidate()
	{
		Offset = Math::MaxValue<uint64_t>();
		Length = 0;
	}

	bool ShaderUniformBufferRegion::IsInvalid(uint64_t minimumSize) const
	{
		return Offset == Math::MaxValue<uint64_t>() || Length < minimumSize || BufferIndex == -1;
	}

    VulkanShaderResource::VulkanShaderResource(const ResourceID& id, const string& name, const VulkanShader& shader) :
        GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name), 
		CachedResource(shader.ID, shader.GetVersion())
    {}

    bool VulkanShaderResource::NeedsUpdate(const VulkanShader& shader) const noexcept
    {
        return !_pool.IsValid() || shader.GetVersion() != GetReferenceVersion();
    }

	VulkanShaderResource::~VulkanShaderResource()
	{
		DestroyPool();
		_bufferRegions.clear();

		for (auto& buffer : _uniformBuffers)
			_device->PurgeResource(buffer.Buffer);
	}

	void VulkanShaderResource::Update(const VulkanShader& shader)
	{
		DestroyPool();

		_pool = _device->CreateResource<VulkanDescriptorPool>(FormattedString("{} Descriptor Pool", shader.GetName()), _maxSets, shader.GetDescriptorLayouts());
		UpdateReferenceVersion(shader.GetVersion());
		IncrementVersion();
	}

	std::pair<const ShaderUniformBufferRegion&, VkDescriptorSet> VulkanShaderResource::CreateOrUpdateBuffer(const ShaderUniformData& data, const VulkanSubshader& vulkanSubshader)
	{
		const Subshader& subshader = vulkanSubshader.GetSubshader();

		bool needsUpdate = false;
		uint64_t dataSize = subshader.GetDescriptorDataSize(_device->GetMinimumBufferAlignment());

		if (!_bufferRegions.contains(data.ID))
		{
			_bufferRegions[data.ID] = ShaderUniformBufferRegion();
			needsUpdate = true;
		}

		ShaderUniformBufferRegion& region = _bufferRegions.at(data.ID);

		if (region.IsInvalid(dataSize))
		{
			FindBufferRegion(dataSize, region);
			needsUpdate = true;
		}

		region.Preserve = data.Preserve;

		if (region.Version != data.Version)
			needsUpdate = true;

		if (needsUpdate)
		{
			ShaderUniformBuffer& buffer = _uniformBuffers[region.BufferIndex];

			// Map the buffer data if it isn't already
			if (buffer.MappedMemory == nullptr)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _bufferSize));

			List<char> uniformData = subshader.GetUniformData(data, _device->GetMinimumBufferAlignment());
			char* dst = buffer.MappedMemory + region.Offset;
			std::memcpy(dst, uniformData.Data(), uniformData.Count());
		}

		VkDescriptorSet set = _pool->GetOrAllocateSet(vulkanSubshader.GetDescriptorLayout(), data.ID.ToHash());

		return { region, set };
	}

	Ref<BufferVulkan> VulkanShaderResource::GetUniformBuffer(const ShaderUniformBufferRegion& region)
	{
		return _uniformBuffers[region.BufferIndex].Buffer;
	}

	void VulkanShaderResource::FlushBufferChanges()
	{
		for (uint i = 0; i < _uniformBuffers.Count(); i++)
		{
			ShaderUniformBuffer& buffer = _uniformBuffers[i];

			// Flush the buffer if it's mapped
			if (buffer.MappedMemory != nullptr)
				buffer.Buffer->Unlock();

			buffer.MappedMemory = nullptr;

			// If the buffer is too fragmented, invalidate it to cause a reshuffling of data
			if (static_cast<double>(buffer.FragmentedSize) / buffer.Buffer->GetSize() > _fragmentFlushThreshold)
			{
				LogInfo(_device->GetLogger(), FormattedString("Shader uniform buffer {} is too fragmented. Invalidating for new frame...", i));
				InvalidateBufferRegions(i);
			}
		}

		// Invalidate temporary regions
		for (auto& regionKVP : _bufferRegions)
		{
			if (regionKVP.second.Preserve)
				continue;

			InvalidateBufferRegion(regionKVP.second);
		}
	}

	void VulkanShaderResource::DestroyPool()
	{
		if (!_pool.IsValid())
			return;

		_device->PurgeResource(_pool);
		_pool = Ref<VulkanDescriptorPool>();
	}

	void VulkanShaderResource::InvalidateBufferRegions(uint bufferIndex)
	{
		for (auto& region : _bufferRegions)
		{
			if (region.second.BufferIndex != bufferIndex)
				continue;

			region.second.Invalidate();
		}

		ShaderUniformBuffer& buffer = _uniformBuffers[bufferIndex];
		buffer.CurrentOffset = 0;
		buffer.FragmentedSize = 0;
	}

	void VulkanShaderResource::InvalidateBufferRegion(ShaderUniformBufferRegion& region)
	{
		ShaderUniformBuffer& buffer = _uniformBuffers[region.BufferIndex];
		buffer.FragmentedSize += region.Length;
		region.Invalidate();
	}

	void VulkanShaderResource::FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& region)
	{
		if (region.BufferIndex != -1)
			_uniformBuffers[region.BufferIndex].FragmentedSize += region.Length;

		int freeBufferIndex = -1;

		// Try to find a buffer that can fit this subshader data
		for (int i = 0; i < _uniformBuffers.Count(); i++)
		{
			if (_uniformBuffers[i].GetFreeRegionSize() >= requiredSize)
			{
				freeBufferIndex = i;
				break;
			}
		}

		// Make a new buffer if we can't fit the data in the current one
		if (freeBufferIndex == -1)
		{
			CreateAdditionalBuffer();
			LogInfo(_device->GetLogger(), FormattedString("Created addition shader uniform buffer. New buffer count is {}", _uniformBuffers.Count()));
			freeBufferIndex = static_cast<int>(_uniformBuffers.Count()) - 1;
		}

		Assert(freeBufferIndex != -1);

		ShaderUniformBuffer& buffer = _uniformBuffers[freeBufferIndex];
		region.BufferIndex = freeBufferIndex;
		region.Offset = buffer.CurrentOffset;
		region.Length = requiredSize;

		buffer.CurrentOffset += requiredSize;
	}

	ShaderUniformBuffer& VulkanShaderResource::CreateAdditionalBuffer()
	{
		ShaderUniformBuffer buffer = {};
		buffer.Buffer = _device->CreateResource<BufferVulkan>("Shader Uniform Buffer",
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			_bufferSize,
			true);

		_uniformBuffers.Add(buffer);

		return _uniformBuffers.Last();
	}
}
