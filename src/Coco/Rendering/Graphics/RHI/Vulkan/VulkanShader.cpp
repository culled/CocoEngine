#include "VulkanShader.h"
#include <Coco/Core/Types/Map.h>
#include "GraphicsDeviceVulkan.h"
#include <Coco/Core/IO/File.h>
#include "VulkanUtilities.h"

namespace Coco::Rendering
{
	// TODO: how to calculate this?
	#define MAX_SETS 10

	VulkanShader::VulkanShader(GraphicsDevice* device, const Shader* shader) :
		_device(static_cast<GraphicsDeviceVulkan*>(device))
	{
		List<Subshader> subshaders = shader->GetSubshaders();
		List<VulkanDescriptorLayout> descriptorSetlayouts;

		for (const Subshader& subshader : subshaders)
		{
			for (const auto& subshaderStagesKVP : subshader.StageFiles)
			{
				try
				{
					VulkanShaderStage stage = CreateShaderStage(subshaderStagesKVP.first, subshader.PassName, subshaderStagesKVP.second);
					_shaderStages[subshader.PassName].Add(stage);
				}
				catch (const Exception& ex)
				{
					throw ObjectCreateException(FormattedString("Unable to create shader module for pass \"{}\": {}", subshader.PassName, ex.what()));
				}
			}

			VulkanDescriptorLayout layout = {};
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

	bool VulkanShader::TryGetSubshaderStages(const string& subshaderName, List<VulkanShaderStage>& stages) const noexcept
	{
		if (!_shaderStages.contains(subshaderName))
			return false;

		stages = _shaderStages.at(subshaderName);
		return true;
	}

	bool VulkanShader::TryGetDescriptorSetLayout(const string& subshaderName, VulkanDescriptorLayout& layout) const noexcept
	{
		if (!_descriptorSetLayouts.contains(subshaderName))
			return false;

		layout = _descriptorSetLayouts.at(subshaderName);
		return true;
	}

	VulkanShaderStage VulkanShader::CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file)
	{
		VulkanShaderStage shaderStage = {};
		shaderStage.EntryPointName = subshaderName;
		shaderStage.StageType = stage;

		shaderStage.ShaderModuleCreateInfo = {};
		shaderStage.ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		List<uint8_t> byteCode = File::ReadAllBytes(file);

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}
}
