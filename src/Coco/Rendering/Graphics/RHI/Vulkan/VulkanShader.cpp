#include "VulkanShader.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/IO/File.h>
#include <Coco/Rendering/Shader.h>
#include "Resources/VulkanDescriptorSet.h"
#include "Resources/VulkanDescriptorPool.h"
#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	VulkanShader::VulkanShader(GraphicsDeviceVulkan* device, const Ref<Shader>& shader) : CachedResource(shader->ID, shader->GetVersion()),
		_device(device), _shader(shader)
	{}

	VulkanShader::~VulkanShader()
	{
		DestroyShaderObjects();
	}

	bool VulkanShader::NeedsUpdate() const noexcept
	{
		if (Ref<Shader> shader = _shader.lock())
			return _subshaders.size() == 0 || this->GetVersion() != shader->GetVersion();

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
			VulkanSubshader vulkanSubshader = {};

			// Load subshader modules
			for (const auto& subshaderStagesKVP : subshader.StageFiles)
			{
				try
				{
					vulkanSubshader.ShaderStages.Add(CreateShaderStage(subshaderStagesKVP.first, subshader.PassName, subshaderStagesKVP.second));
				}
				catch (const Exception& ex)
				{
					throw VulkanRenderingException(FormattedString("Unable to create shader module for pass \"{}\": {}", subshader.PassName, ex.what()));
				}
			}

			// Create subshader descriptor layout
			vulkanSubshader.DescriptorLayout.LayoutBindings.Resize(subshader.Samplers.Count() + (subshader.Descriptors.Count() > 0 ? 1 : 0));
			uint32_t bindingIndex = 0;

			if (subshader.Descriptors.Count() > 0)
			{
				VkDescriptorSetLayoutBinding& binding = vulkanSubshader.DescriptorLayout.LayoutBindings[bindingIndex];
				binding.descriptorCount = 1;
				binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				binding.pImmutableSamplers = nullptr;
				binding.binding = bindingIndex;
				binding.stageFlags = ToVkShaderStageFlagBits(subshader.DescriptorBindingPoint);

				bindingIndex++;
			}

			for (uint32_t i = 0; i < subshader.Samplers.Count(); i++)
			{
				VkDescriptorSetLayoutBinding& binding = vulkanSubshader.DescriptorLayout.LayoutBindings[bindingIndex];
				binding.descriptorCount = 1;
				binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				binding.pImmutableSamplers = nullptr;
				binding.binding = bindingIndex;
				binding.stageFlags = ToVkShaderStageFlagBits(subshader.DescriptorBindingPoint);

				bindingIndex++;
			}

			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(vulkanSubshader.DescriptorLayout.LayoutBindings.Count());
			createInfo.pBindings = vulkanSubshader.DescriptorLayout.LayoutBindings.Data();

			AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &vulkanSubshader.DescriptorLayout.Layout));

			_subshaders.emplace(subshader.PassName, vulkanSubshader);
		}
	}

	bool VulkanShader::TryGetSubshader(const string& subshaderName, const VulkanSubshader*& subshader) const noexcept
	{
		if (!_subshaders.contains(subshaderName))
			return false;

		subshader = &_subshaders.at(subshaderName);
		return true;
	}

	List<VulkanDescriptorLayout> VulkanShader::GetDescriptorLayouts() const noexcept
	{
		List<VulkanDescriptorLayout> layouts(_subshaders.size());

		for (const auto& shaderKVP : _subshaders)
			layouts.Add(shaderKVP.second.DescriptorLayout);

		return layouts;
	}

	void VulkanShader::DestroyShaderObjects() noexcept
	{
		_device->WaitForIdle();

		for (const auto& shaderKVP : _subshaders)
		{
			if(shaderKVP.second.DescriptorLayout.Layout != nullptr)
				vkDestroyDescriptorSetLayout(_device->GetDevice(), shaderKVP.second.DescriptorLayout.Layout, nullptr);

			for (const VulkanShaderStage& stage : shaderKVP.second.ShaderStages)
			{
				if (stage.ShaderModule != nullptr)
				{
					vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
				}
			}
		}

		_subshaders.clear();
	}

	VulkanShaderStage VulkanShader::CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file)
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
