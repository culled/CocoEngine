#include "VulkanShaderResource.h"

#include "VulkanShader.h"
#include "../VulkanDescriptorPool.h"
#include "../../GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Texture.h>
#include "../ImageVulkan.h"
#include "../ImageSamplerVulkan.h"

namespace Coco::Rendering::Vulkan
{
	DescriptorWriteData::DescriptorWriteData(
		ShaderDescriptorScope scope,
		const Subshader& subshader,
		const UnorderedMap<string, ResourceID> textures,
		Ref<BufferVulkan> buffer,
		uint64_t bufferOffset,
		uint64_t bufferLength) : 
		Textures(textures), 
		Uniforms(subshader.GetScopedUniforms(scope)), 
		Samplers(subshader.GetScopedSamplers(scope)), 
		Buffer(buffer), 
		BufferOffset(bufferOffset),
		BufferLength(bufferLength)
	{}

	const ResourceID VulkanShaderResource::_globalUBID = ResourceID::Nil;

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

		_device->PurgeResource(_globalUniformBuffer.Buffer);

		for (auto& buffer : _instanceUniformBuffers)
			_device->PurgeResource(buffer.Buffer);
	}

	void VulkanShaderResource::Update(const VulkanShader& shader)
	{
		DestroyPool();

		_pool = _device->CreateResource<VulkanDescriptorPool>(FormattedString("{} Descriptor Pool", shader.GetName()), _maxSets, shader.GetDescriptorLayouts());
		UpdateReferenceVersion(shader.GetVersion());
		IncrementVersion();
	}

	VkDescriptorSet VulkanShaderResource::PrepareGlobalData(const VulkanSubshader& vulkanSubshader, const RenderView& renderView, const ShaderUniformData& globalData)
	{
		// Use the cached descriptor set if it exists
		const auto it = _descriptorSets.find(_globalUBID);
		if (it != _descriptorSets.cend())
		{
			return it->second;
		}

		const Subshader& subshader = vulkanSubshader.GetSubshader();

		// Early out if no descriptors
		if (!subshader.HasScope(ShaderDescriptorScope::Global))
			return nullptr;

		uint64_t dataSize = subshader.GetDescriptorDataSize(ShaderDescriptorScope::Global, _device->GetMinimumBufferAlignment());

		if (!_globalUniformBuffer.Buffer.IsValid() || _globalUniformBuffer.Buffer->GetSize() != dataSize)
		{
			if (_globalUniformBuffer.Buffer.IsValid())
				_device->PurgeResource(_globalUniformBuffer.Buffer);

			_globalUniformBuffer = CreateBuffer(dataSize);
		}

		// Map the buffer data if it isn't already
		_globalUniformBuffer.MappedMemory = reinterpret_cast<char*>(_globalUniformBuffer.Buffer->Lock(0, dataSize));

		List<char> uniformData = subshader.GetUniformData(ShaderDescriptorScope::Global, globalData, _device->GetMinimumBufferAlignment());

		Assert(uniformData.Count() == dataSize);

		std::memcpy(_globalUniformBuffer.MappedMemory, uniformData.Data(), uniformData.Count());

		_globalUniformBuffer.Buffer->Unlock();

		// Allocate this shader's descriptor set
		VkDescriptorSet set = _pool->GetOrAllocateSet(vulkanSubshader.GetDescriptorLayout(ShaderDescriptorScope::Global), _globalUBID.ToHash());
		_descriptorSets[ResourceID::Nil] = set;

		WriteDescriptorSet(
			set,
			DescriptorWriteData(
				ShaderDescriptorScope::Global,
				subshader,
				globalData.Textures,
				_globalUniformBuffer.Buffer,
				0,
				dataSize),
			renderView);

		return set;
	}

	VkDescriptorSet VulkanShaderResource::PrepareInstanceData(const VulkanSubshader& vulkanSubshader, const RenderView& renderView, const ShaderUniformData& instanceData)
	{
		// Use the cached descriptor set if it exists
		const auto it = _descriptorSets.find(instanceData.ID);
		if (it != _descriptorSets.cend())
		{
			return it->second;
		}

		const Subshader& subshaderInfo = vulkanSubshader.GetSubshader();

		// Early out if no descriptors or samplers
		if (!subshaderInfo.HasScope(ShaderDescriptorScope::Instance))
			return nullptr;

		// Allocate this shader's descriptor set
		VkDescriptorSet set = _pool->GetOrAllocateSet(vulkanSubshader.GetDescriptorLayout(ShaderDescriptorScope::Instance), instanceData.ID.ToHash());
		_descriptorSets[instanceData.ID] = set;

		// Update this instance's region in the instance buffer
		ShaderUniformBufferRegion region = CreateOrUpdateInstanceBuffer(instanceData, vulkanSubshader);
		
		WriteDescriptorSet(
			set, 
			DescriptorWriteData(
				ShaderDescriptorScope::Instance, 
				subshaderInfo, 
				instanceData.Textures, 
				_instanceUniformBuffers[region.BufferIndex].Buffer, 
				region.AllocatedBlock.Offset, 
				region.AllocatedBlock.Size),
			renderView);

		return set;
	}

	ShaderUniformBufferRegion VulkanShaderResource::CreateOrUpdateInstanceBuffer(const ShaderUniformData& data, const VulkanSubshader& vulkanSubshader)
	{
		const Subshader& subshader = vulkanSubshader.GetSubshader();

		bool needsUpdate = false;
		uint64_t dataSize = subshader.GetDescriptorDataSize(ShaderDescriptorScope::Instance, _device->GetMinimumBufferAlignment());

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
			ShaderUniformBuffer& buffer = _instanceUniformBuffers[region.BufferIndex];

			// Map the buffer data if it isn't already
			if (buffer.MappedMemory == nullptr)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _bufferSize));

			List<char> uniformData = subshader.GetUniformData(ShaderDescriptorScope::Instance, data, _device->GetMinimumBufferAlignment());
			char* dst = buffer.MappedMemory + region.AllocatedBlock.Offset;
			std::memcpy(dst, uniformData.Data(), uniformData.Count());

			region.Version = data.Version;
		}

		return region;
	}

	void VulkanShaderResource::WriteDescriptorSet(VkDescriptorSet set, const DescriptorWriteData& data, const RenderView& renderView)
	{
		bool hasUniforms = data.Uniforms.Count() > 0;

		List<VkDescriptorImageInfo> imageInfos(data.Samplers.Count());
		List<VkWriteDescriptorSet> descriptorWrites(imageInfos.Count() + (hasUniforms ? 1 : 0));

		VkDescriptorBufferInfo bufferInfo = {};
		uint32_t bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bufferInfo.buffer = data.Buffer->GetBuffer();
			bufferInfo.offset = data.BufferOffset;
			bufferInfo.range = data.BufferLength;

			write.pBufferInfo = &bufferInfo;

			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < data.Samplers.Count(); i++)
		{
			const ShaderTextureSampler& textureSampler = data.Samplers[i];

			ResourceID textureID = data.Textures.contains(textureSampler.Name) ? data.Textures.at(textureSampler.Name) : Resource::InvalidID;

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			if (textureID != Resource::InvalidID && renderView.Textures.contains(textureID))
			{
				const TextureRenderData& textureData = renderView.Textures.at(textureID);
				image = textureData.Image;
				sampler = textureData.Sampler;
			}
			else
			{
				Ref<Texture> defaultTexture = EnsureRenderingService()->GetDefaultCheckerTexture();
				image = defaultTexture->GetImage();
				sampler = defaultTexture->GetSampler();
			}

			if (!image.IsValid() || !sampler.IsValid())
				throw RenderingException(FormattedString("The sampler or image reference for \"{}\" is empty", textureSampler.Name));

			ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(image.Get());
			ImageSamplerVulkan* vulkanSampler = static_cast<ImageSamplerVulkan*>(sampler.Get());

			// Texture data
			VkDescriptorImageInfo& imageInfo = imageInfos[i];
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetNativeView();
			imageInfo.sampler = vulkanSampler->GetSampler();

			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			bindingIndex++;
		}

		vkUpdateDescriptorSets(_device->GetDevice(), static_cast<uint32_t>(descriptorWrites.Count()), descriptorWrites.Data(), 0, nullptr);
	}

	void VulkanShaderResource::ResetForNewFrame()
	{
		_pool->FreeSets();
		_descriptorSets.clear();
	}

	void VulkanShaderResource::FlushBufferChanges()
	{
		for (uint i = 0; i < _instanceUniformBuffers.Count(); i++)
		{
			ShaderUniformBuffer& buffer = _instanceUniformBuffers[i];

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
		ShaderUniformBuffer& buffer = _instanceUniformBuffers[bufferIndex];

		for (auto& region : _bufferRegions)
		{
			if (region.second.BufferIndex != bufferIndex)
				continue;

			buffer.Buffer->Free(region.second.AllocatedBlock);
		}
	}

	void VulkanShaderResource::FreeBufferRegion(ShaderUniformBufferRegion& region)
	{
		ShaderUniformBuffer& buffer = _instanceUniformBuffers[region.BufferIndex];
		buffer.Buffer->Free(region.AllocatedBlock);
	}

	void VulkanShaderResource::FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& outRegion)
	{
		if (outRegion.BufferIndex != -1)
			FreeBufferRegion(outRegion);

		int freeBufferIndex = -1;

		// Try to find a buffer that can fit this subshader data
		for (int i = 0; i < _instanceUniformBuffers.Count(); i++)
		{
			if (_instanceUniformBuffers[i].Buffer->Allocate(requiredSize, outRegion.AllocatedBlock))
			{
				freeBufferIndex = i;
				break;
			}
		}

		// Make a new buffer if we can't fit the data in the current one
		if (freeBufferIndex == -1)
		{
			_instanceUniformBuffers.Add(CreateBuffer(_bufferSize));
			LogInfo(_device->GetLogger(), FormattedString("Created addition shader uniform buffer. New buffer count is {}", _instanceUniformBuffers.Count()));
			freeBufferIndex = static_cast<int>(_instanceUniformBuffers.Count()) - 1;
			Assert(_instanceUniformBuffers[freeBufferIndex].Buffer->Allocate(requiredSize, outRegion.AllocatedBlock));
		}

		Assert(freeBufferIndex != -1);
		Assert(outRegion.AllocatedBlock.Size >= requiredSize);

		outRegion.BufferIndex = freeBufferIndex;
	}

	ShaderUniformBuffer VulkanShaderResource::CreateBuffer(uint64_t size)
	{
		ShaderUniformBuffer buffer = {};
		buffer.Buffer = _device->CreateResource<BufferVulkan>("Shader Uniform Buffer",
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			size,
			true);

		return buffer;
	}
}
