#include "Renderpch.h"
#include "VulkanUniformData.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanImage.h"
#include "../VulkanImageSampler.h"
#include "../../../../RenderService.h"
#include "../VulkanUtils.h"
#include "VulkanRenderContextCache.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	UniformDataBuffer::UniformDataBuffer(Ref<VulkanBuffer> buffer) :
		Buffer(buffer),
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
		GraphicsDeviceResource<VulkanGraphicsDevice>(MakeKey(passShader)),
		_version(0),
		_uniformBuffers{},
		_pools{},
		_poolCreateInfo{},
		_uniformData{},
		_allocatedSets{},
		_lastUsedTime(0.0)
	{}

	VulkanUniformData::~VulkanUniformData()
	{
		DestroyDescriptorPools();
		_uniformBuffers.clear();
	}

	GraphicsDeviceResourceID VulkanUniformData::MakeKey(const VulkanRenderPassShader& passShader)
	{
		return passShader.ID;
	}

	VkDescriptorSet VulkanUniformData::PrepareData(uint64 instanceID, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader, bool preserve)
	{
		auto it = _allocatedSets.find(instanceID);

		if (it != _allocatedSets.end())
			return it->second;

		const RenderPassShader& shaderInfo = shader.GetInfo();
		UniformScope scope = GetInstanceScope(instanceID);

		if (!shaderInfo.HasScope(scope))
			return nullptr;

		auto uniformIt = _uniformData.find(instanceID);
		bool needsUpdate = false;

		if (uniformIt == _uniformData.end())
		{
			uniformIt = _uniformData.try_emplace(instanceID, instanceID, preserve).first;
			needsUpdate = true;
		}

		AllocatedUniformData& data = uniformIt->second;
		uint32 dataSize = shader.GetUniformDataSize(scope);

		if (dataSize > 0 && dataSize != data.AllocatedBlock.Size)
		{
			if (data.AllocatedBlock.Size != 0)
				FreeBufferRegion(data);

			AllocateBufferRegion(dataSize, data);
			needsUpdate = true;
		}

		if (data.Version == ShaderUniformData::TemporaryVersion || data.Version != uniformData.Version)
			needsUpdate = true;

		VkDescriptorSet set = AllocateDescriptorSet(instanceID, shader.GetDescriptorSetLayout(scope));
		if (!UpdateDescriptorSet(scope, uniformData, shader, data, set))
		{
			return nullptr;
		}

		if (dataSize > 0)
		{
			UniformDataBuffer& buffer = *data.Buffer;

			if (!buffer.MappedMemory)
				buffer.MappedMemory = reinterpret_cast<char*>(buffer.Buffer->Lock(0, _sBufferSize));

			std::vector<uint8> bufferData = GetBufferData(scope, uniformData, shaderInfo);

			char* dst = buffer.MappedMemory + data.AllocatedBlock.Offset;
			Assert(memcpy_s(dst, data.AllocatedBlock.Size, bufferData.data(), bufferData.size()) == 0)
		}

		data.Version = uniformData.Version;

		return set;
	}

	void VulkanUniformData::ResetForNextFrame()
	{
		_allocatedSets.clear();

		_device->WaitForIdle();

		for(auto it = _pools.begin(); it != _pools.end(); it++)
			AssertVkSuccess(vkResetDescriptorPool(_device->GetDevice(), it->Pool, 0))
	}

	void VulkanUniformData::PreparePushConstants(
		VulkanCommandBuffer& commandBuffer, 
		VkPipelineLayout pipelineLayout, 
		const ShaderUniformData& uniformData, 
		const VulkanRenderPassShader& shader)
	{
		const RenderPassShader& shaderInfo = shader.GetInfo();
		if (!shaderInfo.HasScope(UniformScope::Draw))
			return;

		std::vector<uint8> pushConstantData = GetBufferData(UniformScope::Draw, uniformData, shaderInfo);
		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

		std::vector<ShaderDataUniform> uniforms = shaderInfo.GetScopedDataUniforms(UniformScope::Draw);

		for (const ShaderDataUniform& u : uniforms)
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

	void VulkanUniformData::Use()
	{
		_lastUsedTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanUniformData::IsStale() const
	{
		double currentTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanRenderContextCache::sPurgeThreshold;
	}

	std::vector<uint8> VulkanUniformData::GetBufferData(UniformScope scope, const ShaderUniformData& data, const RenderPassShader& shader) const
	{
		std::vector<uint8> bufferData;
		uint64 offset = 0;
		std::vector<ShaderDataUniform> dataUniforms = shader.GetScopedDataUniforms(scope);

		for (const ShaderDataUniform& uniform : dataUniforms)
		{
			const uint8 dataSize = GetDataTypeSize(uniform.Type);
			uint64 preAlignOffset = offset;
			_device->AlignOffset(uniform.Type, offset);

			bufferData.resize(offset + dataSize);
			uint8* dst = bufferData.data() + offset;

			switch (uniform.Type)
			{
			case BufferDataType::Float:
			{
				if (data.Floats.contains(uniform.Key))
				{
					float d = data.Floats.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float2:
			{
				if (data.Float2s.contains(uniform.Key))
				{
					const ShaderUniformData::float2& d = data.Float2s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float3:
			{
				if (data.Float3s.contains(uniform.Key))
				{
					const ShaderUniformData::float3& d = data.Float3s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float4:
			{
				if (data.Float4s.contains(uniform.Key))
				{
					const ShaderUniformData::float4& d = data.Float4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Mat4x4:
			{
				if (data.Mat4x4s.contains(uniform.Key))
				{
					const ShaderUniformData::Mat4x4& d = data.Mat4x4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int:
			{
				if (data.Ints.contains(uniform.Key))
				{
					int32 d = data.Ints.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int2:
			{
				if (data.Int2s.contains(uniform.Key))
				{
					const ShaderUniformData::int2& d = data.Int2s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int3:
			{
				if (data.Int3s.contains(uniform.Key))
				{
					const ShaderUniformData::int3& d = data.Int3s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int4:
			{
				if (data.Int4s.contains(uniform.Key))
				{
					const ShaderUniformData::int4& d = data.Int4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Bool:
			{
				if (data.Bools.contains(uniform.Key))
				{
					uint8 d = data.Bools.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			}

			offset += dataSize;
		}

		// Pad the buffer to fit in the minimum buffer alignment
		bufferData.resize(GraphicsDevice::GetOffsetForAlignment(bufferData.size(), _device->GetMinimumBufferAlignment()));

		return bufferData;
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
			Ref<VulkanBuffer> buffer = _device->CreateBuffer(
				_sBufferSize,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
				true);

			_uniformBuffers.emplace_back(buffer);
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
		AssertVkSuccess(vkCreateDescriptorPool(_device->GetDevice(), &_poolCreateInfo.CreateInfo, _device->GetAllocationCallbacks(), &pool.Pool));

		CocoTrace("Created VulkanDescriptorPool")
		return pool;
	}

	void VulkanUniformData::DestroyDescriptorPool(const VulkanDescriptorPool& pool)
	{
		_device->WaitForIdle();

		vkDestroyDescriptorPool(_device->GetDevice(), pool.Pool, _device->GetAllocationCallbacks());

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

			VkResult poolResult = vkAllocateDescriptorSets(_device->GetDevice(), &setAllocate, &set);

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
		UniformScope scope,
		const ShaderUniformData& uniformData, 
		const VulkanRenderPassShader& shader, 
		const AllocatedUniformData& data,
		VkDescriptorSet& set)
	{
		const RenderPassShader& shaderInfo = shader.GetInfo();

		std::vector<ShaderTextureUniform> textureUniforms = shaderInfo.GetScopedTextureUniforms(scope);
		std::vector<ShaderDataUniform> dataUniforms = shaderInfo.GetScopedDataUniforms(scope);
		bool hasUniforms = dataUniforms.size() > 0;

		std::vector<VkDescriptorImageInfo> imageInfos(textureUniforms.size());
		std::vector<VkWriteDescriptorSet> descriptorWrites(imageInfos.size() + (hasUniforms ? 1 : 0));

		VkDescriptorBufferInfo bufferInfo{};
		uint32 bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bufferInfo.buffer = data.Buffer->Buffer->GetBuffer();
			bufferInfo.offset = data.AllocatedBlock.Offset;
			bufferInfo.range = data.AllocatedBlock.Size;

			write.pBufferInfo = &bufferInfo;

			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < textureUniforms.size(); i++)
		{
			const ShaderTextureUniform& textureSampler = textureUniforms.at(i);

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

		vkUpdateDescriptorSets(_device->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		return true;
	}
}