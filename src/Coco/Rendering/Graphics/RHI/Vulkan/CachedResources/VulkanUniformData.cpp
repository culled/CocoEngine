#include "Renderpch.h"
#include "../../../../RenderService.h"
#include "../../../../Texture.h"
#include "VulkanUniformData.h"
#include "VulkanRenderPassShader.h"
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

	const uint64 VulkanUniformData::sGlobalInstanceID = 0;

	VulkanUniformData::VulkanUniformData(const VulkanRenderPassShader& passShader) :
		CachedVulkanResource(MakeKey(passShader)),
		_version(0),
		_uniformBuffers{},
		_pools{},
		_poolCreateInfo{},
		_uniformData{},
		_allocatedSets{},
		_shaderGlobalBuffers{}
	{}

	VulkanUniformData::~VulkanUniformData()
	{
		DestroyBufferUniforms();
		DestroyDescriptorPools();
		_uniformBuffers.clear();
	}

	GraphicsDeviceResourceID VulkanUniformData::MakeKey(const VulkanRenderPassShader& passShader)
	{
		return passShader.ID;
	}

	void VulkanUniformData::SetBufferUniformData(const VulkanRenderPassShader& shader, ShaderUniformData::UniformKey key, uint64 dataOffset, const void* data, uint64 dataSize)
	{
		if (!_shaderGlobalBuffers.contains(key))
		{
			const RenderPassShader& shaderInfo = shader.GetInfo();
			auto it = std::find_if(shaderInfo.GlobalUniforms.BufferUniforms.begin(), shaderInfo.GlobalUniforms.BufferUniforms.end(), [key](const ShaderBufferUniform& u)
				{
					return u.Key == key;
				});

			Assert(it != shaderInfo.GlobalUniforms.BufferUniforms.end())

			CreateBufferUniform(*it);
		}

		_shaderGlobalBuffers.at(key)->LoadData(dataOffset, data, dataSize);
	}

	VkDescriptorSet VulkanUniformData::PrepareData(uint64 instanceID, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader, bool preserve)
	{
		auto it = _allocatedSets.find(instanceID);

		if (it != _allocatedSets.end())
			return it->second;

		const RenderPassShader& shaderInfo = shader.GetInfo();
		UniformScope scope = GetInstanceScope(instanceID);
		const ShaderUniformLayout& layout = scope == UniformScope::ShaderGlobal ? shaderInfo.GlobalUniforms : shaderInfo.InstanceUniforms;

		if (layout.Hash == ShaderUniformLayout::EmptyHash)
			return nullptr;

		auto uniformIt = _uniformData.find(instanceID);
		bool needsUpdate = false;

		if (uniformIt == _uniformData.end())
		{
			uniformIt = _uniformData.try_emplace(instanceID, instanceID, preserve).first;
			needsUpdate = true;
		}

		AllocatedUniformData& data = uniformIt->second;
		uint64 dataSize = layout.GetUniformDataSize(_device);

		if (dataSize > 0 && dataSize != data.AllocatedBlock.Size)
		{
			if (data.AllocatedBlock.Size != 0)
				FreeBufferRegion(data);

			AllocateBufferRegion(dataSize, data);
			needsUpdate = true;
		}

		if (data.Version == ShaderUniformData::TemporaryVersion || data.Version != uniformData.Version)
			needsUpdate = true;

		const VulkanDescriptorSetLayout& setLayout = shader.GetDescriptorSetLayout(scope);
		VkDescriptorSet set = AllocateDescriptorSet(instanceID, setLayout);
		if (!UpdateDescriptorSet(layout, setLayout, uniformData, data, set))
		{
			return nullptr;
		}

		if (dataSize > 0)
		{
			UniformDataBuffer& buffer = *data.Buffer;

			if (!buffer.MappedMemory)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _sBufferSize));

			std::vector<uint8> bufferData = layout.GetBufferFriendlyData(_device, uniformData);

			char* dst = buffer.MappedMemory + data.AllocatedBlock.Offset;
			Assert(memcpy_s(dst, data.AllocatedBlock.Size, bufferData.data(), bufferData.size()) == 0)
		}

		data.Version = uniformData.Version;

		return set;
	}

	void VulkanUniformData::ResetForNextFrame()
	{
		_allocatedSets.clear();

		_device.WaitForIdle();

		for(auto it = _pools.begin(); it != _pools.end(); it++)
			AssertVkSuccess(vkResetDescriptorPool(_device.GetDevice(), it->Pool, 0))
	}

	void VulkanUniformData::PreparePushConstants(
		VulkanCommandBuffer& commandBuffer, 
		VkPipelineLayout pipelineLayout, 
		const ShaderUniformData& uniformData, 
		const VulkanRenderPassShader& shader)
	{
		const RenderPassShader& shaderInfo = shader.GetInfo();
		if (shaderInfo.DrawUniforms.Hash == ShaderUniformLayout::EmptyHash)
			return;

		std::vector<uint8> pushConstantData = shaderInfo.DrawUniforms.GetBufferFriendlyData(_device, uniformData);
		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

		for (const ShaderDataUniform& u : shaderInfo.DrawUniforms.DataUniforms)
		{
			VkShaderStageFlags uniformStage = ToVkShaderStageFlags(u.BindingPoints);

			stageFlags = Math::Min(stageFlags, uniformStage);
		}

		vkCmdPushConstants(commandBuffer.GetCmdBuffer(), pipelineLayout, stageFlags, 0, static_cast<uint32_t>(pushConstantData.size()), pushConstantData.data());
	}

	bool VulkanUniformData::NeedsUpdate(const VulkanRenderPassShader& passShader) const
	{
		return _version != passShader.GetVersion() || _pools.size() == 0;
	}

	void VulkanUniformData::Update(const VulkanRenderPassShader& passShader)
	{
		DestroyBufferUniforms();
		DestroyDescriptorPools();
		FreeAllBufferRegions();

		for (const VulkanDescriptorSetLayout& layout : passShader.GetDescriptorSetLayouts())
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

		CreateDescriptorPool();

		_version = passShader.GetVersion();
	}

	void VulkanUniformData::AllocateBufferRegion(uint64 requiredSize, AllocatedUniformData& data)
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
			//Ref<VulkanBuffer> buffer = _device.CreateBuffer(
			//	_sBufferSize,
			//	BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			//	true);

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

	void VulkanUniformData::FreeBufferRegion(AllocatedUniformData& data)
	{
		data.Buffer->Buffer->Free(data.AllocatedBlock);

		data.AllocatedBlock.Offset = 0;
		data.AllocatedBlock.Size = 0;
		data.Buffer = _uniformBuffers.end();
	}

	void VulkanUniformData::FreeAllBufferRegions()
	{
		for (UniformDataBuffer& buffer : _uniformBuffers)
		{
			buffer.Buffer->FreeAllAllocations();
		}

		_uniformData.clear();
		_allocatedSets.clear();
	}

	VulkanDescriptorPool& VulkanUniformData::CreateDescriptorPool()
	{
		VulkanDescriptorPool& pool = _pools.emplace_back();
		AssertVkSuccess(vkCreateDescriptorPool(_device.GetDevice(), &_poolCreateInfo.CreateInfo, _device.GetAllocationCallbacks(), &pool.Pool));

		CocoTrace("Created VulkanDescriptorPool")
		return pool;
	}

	void VulkanUniformData::DestroyDescriptorPool(const VulkanDescriptorPool& pool)
	{
		_device.WaitForIdle();

		vkDestroyDescriptorPool(_device.GetDevice(), pool.Pool, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanDescriptorPool")
	}

	void VulkanUniformData::DestroyDescriptorPools()
	{
		for (auto it = _pools.begin(); it != _pools.end(); it++)
			DestroyDescriptorPool(*it);

		_pools.clear();
	}

	VkDescriptorSet VulkanUniformData::AllocateDescriptorSet(uint64 instanceID, const VulkanDescriptorSetLayout& layout)
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

		_allocatedSets.try_emplace(instanceID, set);
		return set;
	}

	bool VulkanUniformData::UpdateDescriptorSet(
		const ShaderUniformLayout& layout,
		const VulkanDescriptorSetLayout& setLayout, 
		const ShaderUniformData& uniformData, 
		const AllocatedUniformData& data,
		VkDescriptorSet& set)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites = setLayout.WriteTemplates;
		std::vector<VkDescriptorImageInfo> imageInfos(layout.TextureUniforms.size());
		bool hasUniforms = layout.DataUniforms.size() > 0;

		std::vector<VkDescriptorBufferInfo> bufferInfos(hasUniforms ? 1 : 0);
		uint32 bufferIndex = 0;
		uint32 bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkDescriptorBufferInfo& bufferInfo = bufferInfos.at(bufferIndex);
			bufferInfo.buffer = data.Buffer->Buffer->GetBuffer();
			bufferInfo.offset = data.AllocatedBlock.Offset;
			bufferInfo.range = data.AllocatedBlock.Size;

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
				Ref<Texture> defaultTexture;

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

		if (const GlobalShaderUniformLayout* globalLayout = dynamic_cast<const GlobalShaderUniformLayout*>(&layout))
		{
			descriptorWrites.resize(descriptorWrites.size() + globalLayout->BufferUniforms.size());
			bufferInfos.resize(bufferInfos.size() + globalLayout->BufferUniforms.size());

			// Write uniform bindings
			for (uint32_t i = 0; i < globalLayout->BufferUniforms.size(); i++)
			{
				const ShaderBufferUniform& bufferUniform = globalLayout->BufferUniforms.at(i);
				if (!_shaderGlobalBuffers.contains(bufferUniform.Key))
					CreateBufferUniform(bufferUniform);

				Assert(_shaderGlobalBuffers.contains(bufferUniform.Key))

				VkDescriptorBufferInfo& bufferInfo = bufferInfos.at(bufferIndex);
				bufferInfo.buffer = _shaderGlobalBuffers.at(bufferUniform.Key)->GetBuffer();
				bufferInfo.offset = 0;
				bufferInfo.range = bufferUniform.Size;

				VkWriteDescriptorSet& write = descriptorWrites.at(bindingIndex);
				write.dstSet = set;
				write.pBufferInfo = &bufferInfo;

				bufferIndex++;
				bindingIndex++;
			}
		}

		vkUpdateDescriptorSets(_device.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		return true;
	}

	void VulkanUniformData::CreateBufferUniform(const ShaderBufferUniform& uniform)
	{
		if (_shaderGlobalBuffers.contains(uniform.Key))
			return;

		_shaderGlobalBuffers.try_emplace(
			uniform.Key,
			CreateManagedRef<VulkanBuffer>(
				uniform.Key,
				uniform.Size,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
				true
			)
		);
	}

	void VulkanUniformData::DestroyBufferUniforms()
	{
		_shaderGlobalBuffers.clear();
	}
}