#include "VulkanShaderResource.h"

#include "VulkanShader.h"
#include "../VulkanDescriptorPool.h"
#include "../../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
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

		if (region.AllocatedBlock.Size != dataSize)
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
			char* dst = buffer.MappedMemory + region.AllocatedBlock.Offset;
			std::memcpy(dst, uniformData.Data(), uniformData.Count());

			region.Version = data.Version;
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
		}

		// Invalidate temporary regions
		for (auto& regionKVP : _bufferRegions)
		{
			if (regionKVP.second.Preserve)
				continue;

			FreeBufferRegion(regionKVP.second);
		}
	}

	void VulkanShaderResource::DestroyPool()
	{
		if (!_pool.IsValid())
			return;

		_device->PurgeResource(_pool);
		_pool = Ref<VulkanDescriptorPool>();
	}

	void VulkanShaderResource::FreeBufferRegions(uint bufferIndex)
	{
		ShaderUniformBuffer& buffer = _uniformBuffers[bufferIndex];

		for (auto& region : _bufferRegions)
		{
			if (region.second.BufferIndex != bufferIndex)
				continue;

			buffer.Buffer->Free(region.second.AllocatedBlock);
		}
	}

	void VulkanShaderResource::FreeBufferRegion(ShaderUniformBufferRegion& region)
	{
		ShaderUniformBuffer& buffer = _uniformBuffers[region.BufferIndex];
		buffer.Buffer->Free(region.AllocatedBlock);
	}

	void VulkanShaderResource::FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& region)
	{
		if (region.BufferIndex != -1)
			FreeBufferRegion(region);

		int freeBufferIndex = -1;

		// Try to find a buffer that can fit this subshader data
		for (int i = 0; i < _uniformBuffers.Count(); i++)
		{
			if (_uniformBuffers[i].Buffer->Allocate(requiredSize, region.AllocatedBlock))
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
			Assert(_uniformBuffers[freeBufferIndex].Buffer->Allocate(requiredSize, region.AllocatedBlock));
		}

		Assert(freeBufferIndex != -1);
		Assert(region.AllocatedBlock.Size >= requiredSize);

		region.BufferIndex = freeBufferIndex;
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
