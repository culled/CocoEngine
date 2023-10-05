#include "Renderpch.h"
#include "VulkanShaderUniformData.h"
#include "../../../../RenderService.h"
#include "../../../../Texture.h"
#include "VulkanGlobalUniformData.h"
#include "VulkanShaderVariant.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanBuffer.h"
#include "../VulkanCommandBuffer.h"
#include "../VulkanImage.h"
#include "../VulkanImageSampler.h"
#include "../VulkanUtils.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	UniformDataBuffer::UniformDataBuffer(ManagedRef<VulkanBuffer>&& buffer) :
		Buffer(std::move(buffer)),
		MappedMemory(nullptr)
	{}

	AllocatedUniformData::AllocatedUniformData(uint64 id, bool preserve) :
		ID(id),
		Version(0),
		Buffer(),
		AllocatedBlock{},
		Preserve(preserve)
	{}

	VulkanDescriptorPoolCreateInfo::VulkanDescriptorPoolCreateInfo() :
		PoolSizes{},
		CreateInfo{}
	{}

	VulkanDescriptorPool::VulkanDescriptorPool() :
		Pool(nullptr),
		LastAllocateTime(0.0)
	{}

	VulkanShaderUniformData::VulkanShaderUniformData(const VulkanShaderVariant& shader) :
		CachedVulkanResource(MakeKey(shader)),
		_version(0),
		_uniformBuffers{},
		_pools{},
		_poolCreateInfo{},
		_uniformData{},
		_instanceSets{}
	{}

	VulkanShaderUniformData::~VulkanShaderUniformData()
	{
		_globalUniformData.reset();
		DestroyDescriptorPools();
		FreeAllBufferRegions();
		_uniformBuffers.clear();
	}

	GraphicsDeviceResourceID VulkanShaderUniformData::MakeKey(const VulkanShaderVariant& shader)
	{
		return shader.ID;
	}

	bool VulkanShaderUniformData::NeedsUpdate(const VulkanShaderVariant& shader) const
	{
		return _version != shader.GetVersion() || _pools.size() == 0;
	}

	void VulkanShaderUniformData::Update(const VulkanShaderVariant& shader)
	{
		_globalUniformData.reset();
		DestroyDescriptorPools();
		FreeAllBufferRegions();

		if (shader.HasScope(UniformScope::ShaderGlobal))
		{
			_globalUniformData.emplace(shader.GetVariant().GlobalUniforms, &shader.GetDescriptorSetLayout(UniformScope::ShaderGlobal));
		}

		for (const VulkanDescriptorSetLayout& layout : shader.GetDescriptorSetLayouts())
		{
			for (const VkDescriptorSetLayoutBinding& binding : layout.LayoutBindings)
			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = binding.descriptorType;
				poolSize.descriptorCount = layout.GetTypeCount(binding.descriptorType) * _sMaxSets;

				_poolCreateInfo.PoolSizes.push_back(poolSize);
			}
		}

		_poolCreateInfo.CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		_poolCreateInfo.CreateInfo.poolSizeCount = static_cast<uint32_t>(_poolCreateInfo.PoolSizes.size());
		_poolCreateInfo.CreateInfo.pPoolSizes = _poolCreateInfo.PoolSizes.data();
		_poolCreateInfo.CreateInfo.maxSets = _sMaxSets;

		if(_poolCreateInfo.PoolSizes.size() > 0)
			CreateDescriptorPool();

		_version = shader.GetVersion();
	}

	void VulkanShaderUniformData::SetBufferUniformData(ShaderUniformData::UniformKey key, uint64 dataOffset, const void* data, uint64 dataSize)
	{
		Assert(_globalUniformData.has_value())

		_globalUniformData->SetBufferUniformData(key, dataOffset, data, dataSize);
	}

	VkDescriptorSet VulkanShaderUniformData::PrepareGlobalData(const ShaderUniformData& uniformData)
	{
		Assert(_globalUniformData.has_value())

		return _globalUniformData->PrepareData(uniformData);
	}

	VkDescriptorSet VulkanShaderUniformData::PrepareInstanceData(const VulkanShaderVariant& shader, uint64 instanceID, const ShaderUniformData& uniformData, bool preserve)
	{
		auto it = _instanceSets.find(instanceID);

		if (it != _instanceSets.end())
			return it->second;

		if (!shader.HasScope(UniformScope::Instance))
			return nullptr;

		auto uniformIt = _uniformData.find(instanceID);
		bool needsUpdate = false;

		if (uniformIt == _uniformData.end())
		{
			uniformIt = _uniformData.try_emplace(instanceID, instanceID, preserve).first;
			needsUpdate = true;
		}

		const ShaderUniformLayout& instanceLayout = shader.GetVariant().InstanceUniforms;
		AllocatedUniformData& data = uniformIt->second;
		uint64 dataSize = instanceLayout.GetUniformDataSize(_device);

		if (dataSize > 0 && dataSize != data.AllocatedBlock.Size)
		{
			if (data.AllocatedBlock.Size != 0)
				FreeBufferRegion(data);

			AllocateBufferRegion(dataSize, data);
			needsUpdate = true;
		}

		if (data.Version == ShaderUniformData::TemporaryVersion || data.Version != uniformData.Version)
			needsUpdate = true;

		const VulkanDescriptorSetLayout& setLayout = shader.GetDescriptorSetLayout(UniformScope::Instance);
		VkDescriptorSet set = AllocateDescriptorSet(setLayout);
		if (!UpdateDescriptorSet(instanceLayout, setLayout, uniformData, &data, set))
		{
			return nullptr;
		}

		_instanceSets.try_emplace(instanceID, set);

		if (dataSize > 0)
		{
			UniformDataBuffer& buffer = *data.Buffer;

			if (!buffer.MappedMemory)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _sBufferSize));

			std::vector<uint8> bufferData = instanceLayout.GetBufferFriendlyData(_device, uniformData);

			char* dst = buffer.MappedMemory + data.AllocatedBlock.Offset;
			Assert(memcpy_s(dst, data.AllocatedBlock.Size, bufferData.data(), bufferData.size()) == 0)
		}

		data.Version = uniformData.Version;

		return set;
	}

	VkDescriptorSet VulkanShaderUniformData::PrepareDrawData(
		const VulkanShaderVariant& shader, 
		VulkanCommandBuffer& commandBuffer, 
		const VulkanPipeline& pipeline, 
		const ShaderUniformData& uniformData)
	{
		if (!shader.HasScope(UniformScope::Draw))
			return nullptr;

		const ShaderVariant& variant = shader.GetVariant();

		if (variant.DrawUniforms.GetUniformDataSize(_device) > 0)
		{
			std::vector<uint8> pushConstantData = variant.DrawUniforms.GetBufferFriendlyData(_device, uniformData);
			VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

			for (const ShaderDataUniform& u : variant.DrawUniforms.DataUniforms)
			{
				VkShaderStageFlags uniformStage = ToVkShaderStageFlags(u.BindingPoints);

				stageFlags = Math::Min(stageFlags, uniformStage);
			}

			vkCmdPushConstants(commandBuffer.GetCmdBuffer(), pipeline.GetPipelineLayout(), stageFlags, 0, static_cast<uint32_t>(pushConstantData.size()), pushConstantData.data());
		}

		// Don't bind a descriptor set for draw uniforms if we don't have draw textures
		if (variant.DrawUniforms.TextureUniforms.size() == 0)
			return nullptr;

		const VulkanDescriptorSetLayout& setLayout = shader.GetDescriptorSetLayout(UniformScope::Draw);
		VkDescriptorSet set = AllocateDescriptorSet(setLayout);
		if (!UpdateDescriptorSet(variant.DrawUniforms, setLayout, uniformData, nullptr, set))
		{
			return nullptr;
		}

		return set;
	}

	void VulkanShaderUniformData::ResetForNextFrame()
	{
		_instanceSets.clear();

		_device.WaitForIdle();

		for(auto it = _pools.begin(); it != _pools.end(); it++)
			AssertVkSuccess(vkResetDescriptorPool(_device.GetDevice(), it->Pool, 0))
	}

	void VulkanShaderUniformData::AllocateBufferRegion(uint64 requiredSize, AllocatedUniformData& data)
	{
		UniformDataBufferList::iterator freeBuffer = _uniformBuffers.begin();

		while (freeBuffer != _uniformBuffers.end())
		{
			if (freeBuffer->Buffer->Allocate(requiredSize, data.AllocatedBlock))
				break;

			freeBuffer++;
		}

		if (freeBuffer == _uniformBuffers.end())
		{
			_uniformBuffers.emplace_back(
				CreateManagedRef<VulkanBuffer>(
					ID, 
					_sBufferSize, 
					BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible, 
					true
				)
			);
			freeBuffer = _uniformBuffers.end();
			freeBuffer--;

			Assert(freeBuffer->Buffer->Allocate(requiredSize, data.AllocatedBlock));
		}

		data.Buffer = freeBuffer;
	}

	void VulkanShaderUniformData::FreeBufferRegion(AllocatedUniformData& data)
	{
		data.Buffer->Buffer->Free(data.AllocatedBlock);

		data.AllocatedBlock.Offset = 0;
		data.AllocatedBlock.Size = 0;
		data.Buffer = _uniformBuffers.end();
	}

	void VulkanShaderUniformData::FreeAllBufferRegions()
	{
		for (UniformDataBuffer& buffer : _uniformBuffers)
		{
			buffer.Buffer->FreeAllAllocations();
		}

		_uniformData.clear();
		_instanceSets.clear();
	}

	VulkanDescriptorPool& VulkanShaderUniformData::CreateDescriptorPool()
	{
		VulkanDescriptorPool& pool = _pools.emplace_back();
		AssertVkSuccess(vkCreateDescriptorPool(_device.GetDevice(), &_poolCreateInfo.CreateInfo, _device.GetAllocationCallbacks(), &pool.Pool));

		CocoTrace("Created VulkanDescriptorPool")
		return pool;
	}

	void VulkanShaderUniformData::DestroyDescriptorPool(const VulkanDescriptorPool& pool)
	{
		_device.WaitForIdle();

		vkDestroyDescriptorPool(_device.GetDevice(), pool.Pool, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanDescriptorPool")
	}

	void VulkanShaderUniformData::DestroyDescriptorPools()
	{
		for (auto it = _pools.begin(); it != _pools.end(); it++)
			DestroyDescriptorPool(*it);

		_pools.clear();
	}

	VkDescriptorSet VulkanShaderUniformData::AllocateDescriptorSet(const VulkanDescriptorSetLayout& layout)
	{
		DescriptorPoolList::iterator it = _pools.begin();
		VkDescriptorSet set = nullptr;

		do
		{
			VkDescriptorSetAllocateInfo setAllocate{};
			setAllocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			setAllocate.descriptorPool = it->Pool;
			setAllocate.descriptorSetCount = 1;
			setAllocate.pSetLayouts = &layout.Layout;

			VkResult poolResult = vkAllocateDescriptorSets(_device.GetDevice(), &setAllocate, &set);

			if (poolResult == VK_SUCCESS)
			{
				it->LastAllocateTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
				break;
			}

			if (poolResult == VK_ERROR_OUT_OF_POOL_MEMORY)
			{
				it++;

				if (it == _pools.end())
				{
					CreateDescriptorPool();
					it = _pools.end();
					it--;
				}
			}
			else
			{
				AssertVkSuccess(poolResult)
			}
		} while (!set);

		return set;
	}

	bool VulkanShaderUniformData::UpdateDescriptorSet(
		const ShaderUniformLayout& layout,
		const VulkanDescriptorSetLayout& setLayout, 
		const ShaderUniformData& uniformData, 
		const AllocatedUniformData* data,
		VkDescriptorSet& set)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites = setLayout.WriteTemplates;
		std::vector<VkDescriptorImageInfo> imageInfos(layout.TextureUniforms.size());
		bool hasUniforms = layout.DataUniforms.size() > 0 && data != nullptr;

		std::vector<VkDescriptorBufferInfo> bufferInfos(hasUniforms ? 1 : 0);
		uint32 bufferIndex = 0;
		uint32 bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkDescriptorBufferInfo& bufferInfo = bufferInfos.at(bufferIndex);
			bufferInfo.buffer = data->Buffer->Buffer->GetBuffer();
			bufferInfo.offset = data->AllocatedBlock.Offset;
			bufferInfo.range = data->AllocatedBlock.Size;

			VkWriteDescriptorSet& write = descriptorWrites.at(bindingIndex);
			write.dstSet = set;
			write.pBufferInfo = &bufferInfo;

			bufferIndex++;
			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < layout.TextureUniforms.size(); i++)
		{
			const ShaderTextureUniform& textureSampler = layout.TextureUniforms.at(i);

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			auto it = uniformData.Textures.find(textureSampler.Key);

			if (it != uniformData.Textures.end())
			{
				const ShaderUniformData::TextureSampler& tex = it->second;

				image = tex.first;
				sampler = tex.second;
			}

			if(!image.IsValid())
			{
				RenderService* rendering = RenderService::Get();
				SharedRef<Texture> defaultTexture;

				switch (textureSampler.DefaultTexture)
				{
				case ShaderTextureUniform::DefaultTextureType::White:
					defaultTexture = rendering->GetDefaultDiffuseTexture();
					break;
				case ShaderTextureUniform::DefaultTextureType::Normal:
					defaultTexture = rendering->GetDefaultNormalTexture();
					break;
				case ShaderTextureUniform::DefaultTextureType::Checker:
				default:
					defaultTexture = rendering->GetDefaultCheckerTexture();
					break;
				}

				image = defaultTexture->GetImage();
				sampler = defaultTexture->GetImageSampler();
			}

			if (!image.IsValid() || !sampler.IsValid())
			{
				CocoError("The image or sampler reference for \"{}\" is empty", textureSampler.Name);
				return false;
			}

			VulkanImage* vulkanImage = static_cast<VulkanImage*>(image.Get());
			VulkanImageSampler* vulkanSampler = static_cast<VulkanImageSampler*>(sampler.Get());

			// Texture data
			VkDescriptorImageInfo& imageInfo = imageInfos.at(i);
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetNativeView();
			imageInfo.sampler = vulkanSampler->GetSampler();

			VkWriteDescriptorSet& write = descriptorWrites.at(bindingIndex);
			write.dstSet = set;
			write.pImageInfo = &imageInfo;

			bindingIndex++;
		}

		vkUpdateDescriptorSets(_device.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		return true;
	}
}