#include "VulkanShader.h"

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/IO/File.h>
#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	VulkanShader::VulkanShader(GraphicsDeviceVulkan* device, Ref<Shader> shader) : CachedResource(shader->GetID(), shader->GetVersion()),
		_device(device), _shader(shader)
	{}

	VulkanShader::~VulkanShader()
	{
		DestroyShaderObjects();
	}

	bool VulkanShader::NeedsUpdate() const noexcept
	{
		if (Ref<Shader> shader = _shader.lock())
			return _shaderStages.size() == 0 || _descriptorSetLayouts.size() == 0 || this->Version != shader->GetVersion();

		return false;
	}

	void VulkanShader::Update()
	{
		DestroyShaderObjects();

		Ref<Shader> shader = _shader.lock();
		Assert(shader != nullptr);

		List<Subshader> subshaders = shader->GetSubshaders();

		for (const Subshader& subshader : subshaders)
		{
			// Load subshader modules
			for (const auto& subshaderStagesKVP : subshader.StageFiles)
			{
				try
				{
					VulkanShaderStage stage = CreateShaderStage(subshaderStagesKVP.first, subshader.PassName, subshaderStagesKVP.second);
					_shaderStages[subshader.PassName].Add(stage);
				}
				catch (const Exception& ex)
				{
					throw VulkanRenderingException(FormattedString("Unable to create shader module for pass \"{}\": {}", subshader.PassName, ex.what()));
				}
			}

			// Create subshader descriptor layout
			VulkanDescriptorLayout layout = {};
			layout.LayoutBindings.Resize(subshader.Samplers.Count() + (subshader.Descriptors.Count() > 0 ? 1 : 0));
			uint32_t bindingIndex = 0;

			if (subshader.Descriptors.Count() > 0)
			{
				VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings[bindingIndex];
				binding.descriptorCount = 1;
				binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				binding.pImmutableSamplers = nullptr;
				binding.binding = bindingIndex;
				binding.stageFlags = ToVkShaderStageFlagBits(subshader.DescriptorBindingPoint);

				bindingIndex++;
			}

			for (uint32_t i = 0; i < subshader.Samplers.Count(); i++)
			{
				VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings[bindingIndex];
				binding.descriptorCount = 1;
				binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				binding.pImmutableSamplers = nullptr;
				binding.binding = bindingIndex;
				binding.stageFlags = ToVkShaderStageFlagBits(subshader.DescriptorBindingPoint);

				bindingIndex++;
			}

			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(layout.LayoutBindings.Count());
			createInfo.pBindings = layout.LayoutBindings.Data();

			AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &layout.Layout));

			_descriptorSetLayouts[subshader.PassName] = layout;
		}
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

	List<VulkanDescriptorLayout> VulkanShader::GetDescriptorLayouts() const noexcept
	{
		List<VulkanDescriptorLayout> layouts(_descriptorSetLayouts.size());

		for (const auto& layoutKVP : _descriptorSetLayouts)
			layouts.Add(layoutKVP.second);

		return layouts;
	}

	void VulkanShader::DestroyShaderObjects() noexcept
	{
		_device->WaitForIdle();

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
