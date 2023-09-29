#include "Renderpch.h"
#include "VulkanGlobalUniformData.h"

#include "../../../../Texture.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanBuffer.h"
#include "../VulkanImage.h"
#include "../VulkanImageSampler.h"
#include "../VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanGlobalUniformData::VulkanGlobalUniformData(const GlobalShaderUniformLayout& layout) :
		CachedVulkanResource(layout.Hash),
		_uniformLayout(layout),
		_descriptorSetLayout(),
		_pool(nullptr),
		_set(nullptr)
	{
		CreateDescriptorSetLayout();

		if (_descriptorSetLayout.Layout)
		{
			CreateDescriptorSetPool();
			AllocateDescriptorSet();
		}

		uint64 dataSize = _uniformLayout.GetUniformDataSize(_device);

		if (dataSize > 0)
		{
			_uniformBuffer = CreateManagedRef<VulkanBuffer>(
				ID,
				dataSize,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
				true
			);
		}

		CocoTrace("Created VulkanGlobalUniformData")
	}

	VulkanGlobalUniformData::~VulkanGlobalUniformData()
	{
		_device.WaitForIdle();

		_uniformBuffer.Invalidate();
		_globalBuffers.clear();

		if (_pool)
		{
			vkDestroyDescriptorPool(_device.GetDevice(), _pool, _device.GetAllocationCallbacks());
			_pool = nullptr;
			_set = nullptr;
		}

		if (_descriptorSetLayout.Layout)
		{
			vkDestroyDescriptorSetLayout(_device.GetDevice(), _descriptorSetLayout.Layout, _device.GetAllocationCallbacks());
			_descriptorSetLayout.Layout = nullptr;
			_descriptorSetLayout.LayoutBindings.clear();
			_descriptorSetLayout.WriteTemplates.clear();
		}

		CocoTrace("Destroyed VulkanGlobalUniformData")
	}

	GraphicsDeviceResourceID VulkanGlobalUniformData::MakeKey(const GlobalShaderUniformLayout& layout)
	{
		return layout.Hash;
	}

	void VulkanGlobalUniformData::SetBufferUniformData(ShaderUniformData::UniformKey key, uint64 dataOffset, const void* data, uint64 dataSize)
	{
		if (!_globalBuffers.contains(key))
		{
			auto it = std::find_if(_uniformLayout.BufferUniforms.begin(), _uniformLayout.BufferUniforms.end(), [key](const ShaderBufferUniform& u)
				{
					return u.Key == key;
				});

			Assert(it != _uniformLayout.BufferUniforms.end())

			CreateGlobalBuffer(*it);
		}

		_globalBuffers.at(key)->LoadData(dataOffset, data, dataSize);
	}

	VkDescriptorSet VulkanGlobalUniformData::PrepareData(const ShaderUniformData& uniformData)
	{
		if (!WriteDescriptorSet(uniformData))
		{
			return nullptr;
		}

		if (_uniformBuffer.IsValid())
		{
			std::vector<uint8> bufferData = _uniformLayout.GetBufferFriendlyData(_device, uniformData);

			_uniformBuffer->LoadData<uint8>(0, bufferData);
		}

		return _set;
	}

	void VulkanGlobalUniformData::CreateDescriptorSetLayout()
	{
		if (_uniformLayout.Hash == ShaderUniformLayout::EmptyHash)
			return;

		bool hasDataUniforms = _uniformLayout.DataUniforms.size() > 0;

		_descriptorSetLayout.LayoutBindings.resize(_uniformLayout.TextureUniforms.size() + _uniformLayout.BufferUniforms.size() + (hasDataUniforms ? 1 : 0));
		_descriptorSetLayout.WriteTemplates.resize(_descriptorSetLayout.LayoutBindings.size());
		uint32 bindingIndex = 0;

		if (hasDataUniforms)
		{
			VkDescriptorSetLayoutBinding& binding = _descriptorSetLayout.LayoutBindings.at(bindingIndex);
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(_uniformLayout.GetDataUniformBindStages());

			VkWriteDescriptorSet& write = _descriptorSetLayout.WriteTemplates.at(bindingIndex);
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bindingIndex++;
		}

		for (uint32 i = 0; i < _uniformLayout.TextureUniforms.size(); i++)
		{
			VkDescriptorSetLayoutBinding& binding = _descriptorSetLayout.LayoutBindings.at(bindingIndex);
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(_uniformLayout.TextureUniforms.at(i).BindingPoints);

			VkWriteDescriptorSet& write = _descriptorSetLayout.WriteTemplates.at(bindingIndex);
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;

			bindingIndex++;
		}

		for (uint32 i = 0; i < _uniformLayout.BufferUniforms.size(); i++)
		{
			VkDescriptorSetLayoutBinding& binding = _descriptorSetLayout.LayoutBindings.at(bindingIndex);
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(_uniformLayout.BufferUniforms.at(i).BindingPoints);

			VkWriteDescriptorSet& write = _descriptorSetLayout.WriteTemplates.at(bindingIndex);
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bindingIndex++;
		}

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(_descriptorSetLayout.LayoutBindings.size());
		createInfo.pBindings = _descriptorSetLayout.LayoutBindings.data();

		AssertVkSuccess(vkCreateDescriptorSetLayout(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_descriptorSetLayout.Layout))
	}

	void VulkanGlobalUniformData::CreateDescriptorSetPool()
	{
		VkDescriptorPoolCreateInfo poolCreateInfo{};
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (const VkDescriptorSetLayoutBinding& binding : _descriptorSetLayout.LayoutBindings)
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = binding.descriptorType;
			poolSize.descriptorCount = _descriptorSetLayout.GetTypeCount(binding.descriptorType);

			poolSizes.push_back(poolSize);
		}

		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolCreateInfo.pPoolSizes = poolSizes.data();
		poolCreateInfo.maxSets = 1;

		AssertVkSuccess(vkCreateDescriptorPool(_device.GetDevice(), &poolCreateInfo, _device.GetAllocationCallbacks(), &_pool))
	}

	void VulkanGlobalUniformData::AllocateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo setAllocate{};
		setAllocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocate.descriptorPool = _pool;
		setAllocate.descriptorSetCount = 1;
		setAllocate.pSetLayouts = &_descriptorSetLayout.Layout;

		AssertVkSuccess(vkAllocateDescriptorSets(_device.GetDevice(), &setAllocate, &_set))
	}

	bool VulkanGlobalUniformData::WriteDescriptorSet(const ShaderUniformData& uniformData)
	{
		bool hasUniforms = _uniformLayout.DataUniforms.size() > 0;

		std::vector<VkWriteDescriptorSet> descriptorWrites = _descriptorSetLayout.WriteTemplates;

		std::vector<VkDescriptorBufferInfo> bufferInfos((hasUniforms ? 1 : 0) + _uniformLayout.BufferUniforms.size());
		std::vector<VkDescriptorImageInfo> imageInfos(_uniformLayout.TextureUniforms.size());
		uint32 bufferIndex = 0;
		uint32 bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkDescriptorBufferInfo& bufferInfo = bufferInfos.at(bufferIndex);
			bufferInfo.buffer = _uniformBuffer->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = _uniformBuffer->GetSize();

			VkWriteDescriptorSet& write = descriptorWrites.at(bindingIndex);
			write.dstSet = _set;
			write.pBufferInfo = &bufferInfo;

			bufferIndex++;
			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < _uniformLayout.TextureUniforms.size(); i++)
		{
			const ShaderTextureUniform& textureSampler = _uniformLayout.TextureUniforms.at(i);

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			auto it = uniformData.Textures.find(textureSampler.Key);

			if (it != uniformData.Textures.end())
			{
				const ShaderUniformData::TextureSampler& tex = it->second;

				image = tex.first;
				sampler = tex.second;
			}

			if (!image.IsValid())
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
			write.dstSet = _set;
			write.pImageInfo = &imageInfo;

			bindingIndex++;
		}

		// Write uniform bindings
		for (uint32_t i = 0; i < _uniformLayout.BufferUniforms.size(); i++)
		{
			const ShaderBufferUniform& bufferUniform = _uniformLayout.BufferUniforms.at(i);
			if (!_globalBuffers.contains(bufferUniform.Key))
				CreateGlobalBuffer(bufferUniform);

			Assert(_globalBuffers.contains(bufferUniform.Key))

			VkDescriptorBufferInfo& bufferInfo = bufferInfos.at(bufferIndex);
			bufferInfo.buffer = _globalBuffers.at(bufferUniform.Key)->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = bufferUniform.Size;

			VkWriteDescriptorSet& write = descriptorWrites.at(bindingIndex);
			write.dstSet = _set;
			write.pBufferInfo = &bufferInfo;

			bufferIndex++;
			bindingIndex++;
		}

		vkUpdateDescriptorSets(_device.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		return true;
	}

	void VulkanGlobalUniformData::CreateGlobalBuffer(const ShaderBufferUniform& uniform)
	{
		if (_globalBuffers.contains(uniform.Key))
			return;

		_globalBuffers.try_emplace(
			uniform.Key,
			CreateManagedRef<VulkanBuffer>(
				uniform.Key,
				uniform.Size,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
				true
			)
		);
	}
}