#include "VulkanShader.h"
#include <Coco/Core/Types/Map.h>
#include "GraphicsDeviceVulkan.h"
#include <Coco/Core/IO/File.h>
#include "VulkanUtilities.h"

namespace Coco::Rendering
{
	#define MAX_SETS 10

	VulkanShader::VulkanShader(GraphicsDevice* device, const Ref<Shader>& shader) :
		_device(static_cast<GraphicsDeviceVulkan*>(device))
	{
		List<Subshader> subshaders = shader->GetSubshaders();
		List<VulkanShaderDescriptorLayout> descriptorSetlayouts;

		for (const Subshader& subshader : subshaders)
		{
			for (const auto& subshaderStagesKVP : subshader.StageFiles)
			{
				try
				{
					VulkanShaderStage stage = CreateShaderStage(subshaderStagesKVP.first, subshader.PassName, subshaderStagesKVP.second);
					_shaderStages[subshader.PassName].Add(stage);
				}
				catch (Exception& ex)
				{
					string err = FormattedString("Unable to create shader module for \"{}:{}\": {}", shader->GetName(), subshader.PassName, ex.what());
					throw Exception(err.c_str());
				}
			}


			VulkanShaderDescriptorLayout layout = {};
			layout.LayoutBindings.Resize(subshader.Descriptors.Count());

			for (uint32_t i = 0; i < subshader.Descriptors.Count(); i++)
			{
				VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings[i];
				binding.binding = i;
				binding.descriptorCount = 1;
				binding.descriptorType = ToVkDescriptorType(subshader.Descriptors[i].Type);
				binding.pImmutableSamplers = nullptr;
				binding.stageFlags = ToVkShaderStageFlagBits(subshader.Descriptors[i].StageBindingPoint);
			}

			// Descriptor sets
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(layout.LayoutBindings.Count());
			createInfo.pBindings = layout.LayoutBindings.Data();

			AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &layout.Layout));

			_descriptorSetLayouts[subshader.PassName] = layout;
			descriptorSetlayouts.Add(layout);
		}

		_descriptorPool = _device->CreateResource<DescriptorPoolVulkan>(MAX_SETS, descriptorSetlayouts);
	}

	VulkanShader::~VulkanShader()
	{
		_descriptorPool.reset();

		for (const auto& layoutKVP : _descriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(_device->GetDevice(), layoutKVP.second.Layout, nullptr);
		}

		_descriptorSetLayouts.clear();

		for (const auto& shaderKVP : _shaderStages)
		{
			for (const VulkanShaderStage& stage : shaderKVP.second)
			{
				if (stage.ShaderModule != nullptr)
				{
					vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
				}
			}
		}

		_shaderStages.clear();
	}

	List<VulkanShaderStage> VulkanShader::GetSubshaderStages(const string& subshaderName) const
	{
		if (!_shaderStages.contains(subshaderName))
			return List<VulkanShaderStage>();

		return _shaderStages.at(subshaderName);
	}

	VulkanShaderDescriptorLayout VulkanShader::GetDescriptorSetLayout(const string& subshaderName) const
	{
		if (!_descriptorSetLayouts.contains(subshaderName))
			throw Exception("No layout was created for subshader");

		return _descriptorSetLayouts.at(subshaderName);
	}

	VulkanShaderStage VulkanShader::CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file)
	{
		VulkanShaderStage shaderStage = {};
		shaderStage.EntryPointName = subshaderName;
		shaderStage.StageType = stage;

		shaderStage.ShaderModuleCreateInfo = {};
		shaderStage.ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		List<uint8_t> byteCode;

		try
		{
			byteCode = File::ReadAllBytes(file);
		}
		catch (Exception& ex)
		{
			string err = FormattedString("Unable to read shader file: {}", ex.what());
			throw Exception(err.c_str());
		}

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}
}
