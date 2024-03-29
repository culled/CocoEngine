#include "Renderpch.h"
#include "VulkanShaderUniformData.h"
#include "../../../../RenderService.h"
#include "../../../../Texture.h"
#include "VulkanGlobalUniformData.h"
#include "VulkanShader.h"
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

	VulkanShaderUniformData::VulkanShaderUniformData(const VulkanShader& shader) :
		CachedVulkanResource(MakeKey(shader)),
		_version(Math::MaxValue<uint64>()),
		_uniformBuffers{},
		_pools{},
		_poolCreateInfo{},
		_uniformData{},
		_instanceSets{},
		_uniformDataBuffer()
	{}

	VulkanShaderUniformData::~VulkanShaderUniformData()
	{
		_uniformDataBuffer.clear();
		_globalUniformData.reset();

		DestroyDescriptorPools();
		FreeAllBufferRegions();

		_uniformBuffers.clear();
	}

	GraphicsDeviceResourceID VulkanShaderUniformData::MakeKey(const VulkanShader& shader)
	{
		return shader.ID;
	}

	bool VulkanShaderUniformData::NeedsUpdate(const VulkanShader& shader) const
	{
		return _version != shader.GetVersion();
	}

	void VulkanShaderUniformData::Update(const VulkanShader& shader)
	{
		_globalUniformData.reset();
		DestroyDescriptorPools();
		FreeAllBufferRegions();

		if (shader.HasScope(UniformScope::ShaderGlobal))
		{
			_globalUniformData.emplace(shader.GetBaseShader()->GetGlobalUniformLayout(), &shader.GetDescriptorSetLayout(UniformScope::ShaderGlobal));
		}

		uint64 dataSize = 0;

		for (const auto& layoutKVP : shader.GetDescriptorSetLayouts())
		{
			const VulkanDescriptorSetLayout& layout = layoutKVP.second;

			for (const VkDescriptorSetLayoutBinding& binding : layout.LayoutBindings)
			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = binding.descriptorType;
				poolSize.descriptorCount = layout.GetTypeCount(binding.descriptorType) * _sMaxSets;

				_poolCreateInfo.PoolSizes.push_back(poolSize);
			}

			dataSize = Math::Max(dataSize, layout.DataSize);
		}

		_uniformDataBuffer.reserve(dataSize);

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

	VkDescriptorSet VulkanShaderUniformData::PrepareInstanceData(const VulkanShader& shader, uint64 instanceID, const ShaderUniformData& uniformData, bool preserve)
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

		const ShaderUniformLayout& instanceLayout = shader.GetBaseShader()->GetInstanceUniformLayout();
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

		if (needsUpdate && dataSize > 0)
		{
			UniformDataBuffer& buffer = *data.Buffer;

			if (!buffer.MappedMemory)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _sBufferSize));

			_uniformDataBuffer.clear();
			instanceLayout.GetBufferFriendlyData(_device, uniformData, _uniformDataBuffer);

			char* dst = buffer.MappedMemory + data.AllocatedBlock.Offset;
			Assert(memcpy_s(dst, data.AllocatedBlock.Size, _uniformDataBuffer.data(), _uniformDataBuffer.size()) == 0)
		}

		data.Version = uniformData.Version;

		return set;
	}

	VkDescriptorSet VulkanShaderUniformData::PrepareDrawData(
		const VulkanShader& shader, 
		VulkanCommandBuffer& commandBuffer, 
		const VulkanPipeline& pipeline, 
		const ShaderUniformData& uniformData)
	{
		if (!shader.HasScope(UniformScope::Draw))
			return nullptr;

		const auto& drawUniforms = shader.GetBaseShader()->GetDrawUniformLayout();

		if (drawUniforms.HasDataUniforms())
		{
			_uniformDataBuffer.clear();
			drawUniforms.GetBufferFriendlyData(_device, uniformData, _uniformDataBuffer);
			VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

			for (const ShaderUniform& u : drawUniforms.Uniforms)
			{
				if (!IsDataShaderUniformType(u.Type))
					continue;

				VkShaderStageFlags uniformStage = ToVkShaderStageFlags(u.BindingPoints);

				stageFlags = Math::Min(stageFlags, uniformStage);
			}

			vkCmdPushConstants(commandBuffer.GetCmdBuffer(), pipeline.GetPipelineLayout(), stageFlags, 0, static_cast<uint32_t>(_uniformDataBuffer.size()), _uniformDataBuffer.data());
		}

		// Don't bind a descriptor set for draw uniforms if we don't have draw textures
		if (!drawUniforms.HasTextureUniforms())
			return nullptr;

		const VulkanDescriptorSetLayout& setLayout = shader.GetDescriptorSetLayout(UniformScope::Draw);
		VkDescriptorSet set = AllocateDescriptorSet(setLayout);
		if (!UpdateDescriptorSet(drawUniforms, setLayout, uniformData, nullptr, set))
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

			++freeBuffer;
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
			--freeBuffer;

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
				++it;

				if (it == _pools.end())
				{
					CreateDescriptorPool();
					it = _pools.end();
					--it;
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
		std::vector<VkDescriptorImageInfo> imageInfos;
		imageInfos.reserve(descriptorWrites.size());

		bool hasUniforms = layout.HasDataUniforms() && data != nullptr;

		std::vector<VkDescriptorBufferInfo> bufferInfos;
		uint32 bufferIndex = 0;
		uint32 bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
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
		for (const ShaderUniform& uniform : layout.Uniforms)
		{
			if (uniform.Type != ShaderUniformType::Texture)
				continue;

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			auto it = uniformData.Textures.find(uniform.Key);

			if (it != uniformData.Textures.end())
			{
				const ShaderUniformData::TextureSampler& tex = it->second;

				image = tex.first;
				sampler = tex.second;
			}

			if(!image.IsValid())
			{
				const RenderService& rendering = *RenderService::Get();
				SharedRef<Texture> defaultTexture;

				DefaultTextureType textureType = DefaultTextureType::Checkered;

				if (const DefaultTextureType* t = std::any_cast<DefaultTextureType>(&uniform.DefaultValue))
					textureType = *t;

				switch (textureType)
				{
				case DefaultTextureType::White:
					defaultTexture = rendering.GetDefaultDiffuseTexture();
					break;
				case DefaultTextureType::FlatNormal:
					defaultTexture = rendering.GetDefaultNormalTexture();
					break;
				case DefaultTextureType::Checkered:
				default:
					defaultTexture = rendering.GetDefaultCheckerTexture();
					break;
				}

				image = defaultTexture->GetImage();
				sampler = defaultTexture->GetImageSampler();
			}

			if (!image.IsValid() || !sampler.IsValid())
			{
				CocoError("The image or sampler reference for \"{}\" is empty", uniform.Name);
				return false;
			}

			VulkanImage* vulkanImage = static_cast<VulkanImage*>(image.Get());
			VulkanImageSampler* vulkanSampler = static_cast<VulkanImageSampler*>(sampler.Get());

			// Texture data
			VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
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