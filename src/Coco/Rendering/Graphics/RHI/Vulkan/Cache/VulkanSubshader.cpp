#include "VulkanSubshader.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/File.h>
#include "../../../RenderView.h"
#include "../GraphicsDeviceVulkan.h"
#include "../VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	VulkanSubshader::VulkanSubshader(GraphicsDeviceVulkan* device, const ShaderRenderData& shaderData, const string& subshaderName) :
		_device(device), ShaderID(shaderData.ID), _shaderVersion(shaderData.Version), SubshaderName(subshaderName)
	{
		_subshader = shaderData.Subshaders.at(subshaderName);
	}

	VulkanSubshader::~VulkanSubshader()
	{
		DestroyShaderObjects();
	}

	bool VulkanSubshader::NeedsUpdate(const ShaderRenderData& shaderData) const noexcept
	{
		return _shaderStages.Count() == 0 || _shaderVersion != shaderData.Version || _descriptorLayout.Layout == nullptr;
	}

	void VulkanSubshader::Update(const ShaderRenderData& shaderData)
	{
		DestroyShaderObjects();

		_subshader = shaderData.Subshaders.at(SubshaderName);

		// Load subshader modules
		for (const auto& subshaderStagesKVP : _subshader.StageFiles)
		{
			try
			{
				_shaderStages.Add(CreateShaderStage(subshaderStagesKVP.first, _subshader.PassName, subshaderStagesKVP.second));
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Unable to create shader module for pass \"{}\": {}", _subshader.PassName, ex.what()));
			}
		}

		// Create subshader descriptor layout
		_descriptorLayout.LayoutBindings.Resize(_subshader.Samplers.Count() + (_subshader.Descriptors.Count() > 0 ? 1 : 0));
		uint32_t bindingIndex = 0;

		if (_subshader.Descriptors.Count() > 0)
		{
			VkDescriptorSetLayoutBinding& binding = _descriptorLayout.LayoutBindings[bindingIndex];
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlagBits(_subshader.DescriptorBindingPoint);

			bindingIndex++;
		}

		for (uint32_t i = 0; i < _subshader.Samplers.Count(); i++)
		{
			VkDescriptorSetLayoutBinding& binding = _descriptorLayout.LayoutBindings[bindingIndex];
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlagBits(_subshader.DescriptorBindingPoint);

			bindingIndex++;
		}

		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(_descriptorLayout.LayoutBindings.Count());
		createInfo.pBindings = _descriptorLayout.LayoutBindings.Data();

		AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &_descriptorLayout.Layout));

		_shaderVersion = shaderData.Version;
	}

	void VulkanSubshader::DestroyShaderObjects() noexcept
	{
		_device->WaitForIdle();

		if(_descriptorLayout.Layout != nullptr)
			vkDestroyDescriptorSetLayout(_device->GetDevice(), _descriptorLayout.Layout, nullptr);

		for (const VulkanShaderStage& stage : _shaderStages)
		{
			if (stage.ShaderModule != nullptr)
			{
				vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
			}
		}
	}

	VulkanShaderStage VulkanSubshader::CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file)
	{
		VulkanShaderStage shaderStage = {};
		shaderStage.EntryPointName = subshaderName;
		shaderStage.StageType = stage;

		shaderStage.ShaderModuleCreateInfo = {};
		shaderStage.ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		const string fullFilePath = Engine::Get()->GetResourceLibrary()->GetFullFilePath(file);
		List<uint8_t> byteCode = File::ReadAllBytes(fullFilePath);

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}
}
