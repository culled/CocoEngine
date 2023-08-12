#include "VulkanShader.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/File.h>
#include <Coco/Rendering/Graphics/RenderView.h>
#include "../../GraphicsDeviceVulkan.h"
#include "../../VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{

	VulkanSubshader::VulkanSubshader(GraphicsDeviceVulkan* device, const Subshader& subshaderData) :
		_device(device), _name(subshaderData.PassName), _subshader(subshaderData), _isValid(true)
	{
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
	}

	VulkanSubshader::VulkanSubshader(VulkanSubshader&& other) noexcept :
		_name(std::move(other._name)),
		_device(std::move(other._device)),
		_shaderStages(std::move(other._shaderStages)),
		_descriptorLayout(std::move(other._descriptorLayout)),
		_subshader(std::move(other._subshader)), 
		_isValid(true)
	{
		other._isValid = false;
	}

	VulkanSubshader& VulkanSubshader::operator=(VulkanSubshader&& other) noexcept
	{
		_name = std::move(other._name);
		_device = std::move(other._device);
		_shaderStages = std::move(other._shaderStages);
		_descriptorLayout = std::move(other._descriptorLayout);
		_subshader = std::move(other._subshader);
		_isValid = true;
		other._isValid = false;

		return *this;
	}

	VulkanSubshader::~VulkanSubshader()
	{
		if(_isValid)
			DestroyShaderObjects();
	}

	void VulkanSubshader::DestroyShaderObjects() noexcept
	{
		_device->WaitForIdle();

		if (_descriptorLayout.Layout != nullptr)
		{
			vkDestroyDescriptorSetLayout(_device->GetDevice(), _descriptorLayout.Layout, nullptr);
			_descriptorLayout.Layout = nullptr;
		}

		for (VulkanShaderStage& stage : _shaderStages)
		{
			if (stage.ShaderModule != nullptr)
			{
				vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
			}

			stage.ShaderModule = nullptr;
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
		List<char> byteCode = File::ReadAllBytes(fullFilePath);

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}

	VulkanShader::VulkanShader(const ResourceID& id, const string& name, const ShaderRenderData& shaderData) :
		GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name), 
		CachedResource(shaderData.ID, shaderData.Version)
	{}

	bool VulkanShader::NeedsUpdate(const ShaderRenderData& shaderData) const noexcept
	{
		return GetReferenceVersion() != shaderData.Version || _subshaders.size() == 0 ||
			std::any_of(_subshaders.cbegin(), _subshaders.cend(), [](const auto& kvp) {
				return kvp.second._shaderStages.Count() == 0 || kvp.second._descriptorLayout.Layout == nullptr;
			});
	}

	void VulkanShader::Update(const ShaderRenderData& shaderData)
	{
		_subshaders.clear();

		// Recreate all subshaders
		for (const auto& subshaderData : shaderData.Subshaders)
		{
			_subshaders.try_emplace(subshaderData.first, _device, subshaderData.second);
		}

		UpdateReferenceVersion(shaderData.Version);
		IncrementVersion();
	}

	bool VulkanShader::TryGetSubshader(const string& name, const VulkanSubshader*& subshader) const
	{
		if (!_subshaders.contains(name))
			return false;

		subshader = &_subshaders.at(name);
		return true;
	}
	
	List<VulkanDescriptorLayout> VulkanShader::GetDescriptorLayouts() const
	{
		List<VulkanDescriptorLayout> layouts;

		for (auto& subshader : _subshaders)
			layouts.Add(subshader.second.GetDescriptorLayout());

		return layouts;
	}
}
