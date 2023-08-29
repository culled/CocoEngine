#include "VulkanShader.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/File.h>
#include <Coco/Rendering/Graphics/RenderView.h>
#include "../../GraphicsDeviceVulkan.h"
#include "../../VulkanUtilities.h"
#include <Coco/Rendering/RenderingUtilities.h>

namespace Coco::Rendering::Vulkan
{
	VulkanShaderStage::VulkanShaderStage() : ShaderStage(), ShaderModuleCreateInfo{}
	{}

	VulkanSubshader::VulkanSubshader(GraphicsDeviceVulkan* device, const Subshader& subshaderData) :
		_device(device), _name(subshaderData.PassName), _subshader(subshaderData), _isValid(true)
	{
		// Load subshader modules
		for (const auto& subshaderStage : _subshader.Stages)
		{
			try
			{
				_shaderStages.Add(CreateShaderStage(subshaderStage));
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Unable to create shader module for pass \"{}\": {}", _subshader.PassName, ex.what()));
			}
		}

		if (_subshader.HasScope(ShaderDescriptorScope::Global))
			_descriptorLayouts.try_emplace(ShaderDescriptorScope::Global, CreateDescriptorLayout(ShaderDescriptorScope::Global));

		if (_subshader.HasScope(ShaderDescriptorScope::Instance))
			_descriptorLayouts.try_emplace(ShaderDescriptorScope::Instance, CreateDescriptorLayout(ShaderDescriptorScope::Instance));
	}

	VulkanSubshader::VulkanSubshader(VulkanSubshader&& other) noexcept :
		_name(std::move(other._name)),
		_device(std::move(other._device)),
		_shaderStages(std::move(other._shaderStages)),
		_descriptorLayouts(std::move(other._descriptorLayouts)),
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
		_descriptorLayouts = std::move(other._descriptorLayouts);
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

	List<VulkanDescriptorLayout> VulkanSubshader::GetDescriptorLayouts() const noexcept
	{
		List<VulkanDescriptorLayout> layouts;

		if (_descriptorLayouts.contains(ShaderDescriptorScope::Global))
			layouts.Add(GetDescriptorLayout(ShaderDescriptorScope::Global));

		if (_descriptorLayouts.contains(ShaderDescriptorScope::Instance))
			layouts.Add(GetDescriptorLayout(ShaderDescriptorScope::Instance));

		return layouts;
	}

	uint VulkanSubshader::GetUniformDataSize(ShaderDescriptorScope scope) const
	{
		uint64_t offset = 0;
		List<ShaderUniformDescriptor> uniforms = _subshader.GetScopedUniforms(scope);

		for (const auto& uniform : uniforms)
		{
			_device->AlignOffset(uniform.Type, offset);
			offset += GetBufferDataFormatSize(uniform.Type);
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		offset = RenderingUtilities::GetOffsetForAlignment(offset, _device->GetMinimumBufferAlignment());

		return static_cast<uint>(offset);
	}

	List<VkPushConstantRange> VulkanSubshader::GetPushConstantRanges() const
	{
		const uint dataSize = GetUniformDataSize(ShaderDescriptorScope::Draw);

		// TODO: check maximum push constant range size
		if (dataSize > 128)
		{
			LogError(_device->GetLogger(), FormattedString(
				"Cannot have a push constant buffer greater than 128 bytes. Requested: {} bytes",
				dataSize
			));

			return List<VkPushConstantRange>();
		}

		List<ShaderUniformDescriptor> drawUniforms = _subshader.GetScopedUniforms(ShaderDescriptorScope::Draw);

		List<VkPushConstantRange> ranges(drawUniforms.Count());

		uint64_t offset = 0;

		for (int i = 0; i < drawUniforms.Count(); i++)
		{
			const ShaderUniformDescriptor& uniform = drawUniforms[i];
			const uint32_t size = GetBufferDataFormatSize(uniform.Type);

			_device->AlignOffset(uniform.Type, offset);

			VkPushConstantRange& range = ranges[i];

			range.offset = static_cast<uint32_t>(offset);
			range.size = size;
			range.stageFlags = ToVkShaderStageFlagBits(uniform.BindingPoints);

			offset += size;
		}

		return ranges;
	}

	void VulkanSubshader::DestroyShaderObjects() noexcept
	{
		_device->WaitForIdle();

		for (auto& kvp : _descriptorLayouts)
		{
			VulkanDescriptorLayout& layout = kvp.second;
			if (layout.Layout != nullptr)
			{
				vkDestroyDescriptorSetLayout(_device->GetDevice(), layout.Layout, nullptr);
				layout.Layout = nullptr;
			}
		}

		_descriptorLayouts.clear();

		for (VulkanShaderStage& stage : _shaderStages)
		{
			if (stage.ShaderModule != nullptr)
			{
				vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
			}

			stage.ShaderModule = nullptr;
		}
	}

	VulkanShaderStage VulkanSubshader::CreateShaderStage(const ShaderStage& stage)
	{
		VulkanShaderStage shaderStage = {};
		shaderStage.Type = stage.Type;
		shaderStage.EntryPointName = stage.EntryPointName;
		shaderStage.FilePath = stage.FilePath;

		shaderStage.ShaderModuleCreateInfo = {};
		shaderStage.ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		// TODO: maybe store the full path in the shader stage after loading?
		const string fullFilePath = Engine::Get()->GetResourceLibrary()->GetFullFilePath(shaderStage.FilePath);
		List<char> byteCode = File::ReadAllBytes(fullFilePath);

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}

	VulkanDescriptorLayout VulkanSubshader::CreateDescriptorLayout(ShaderDescriptorScope scope)
	{
		VulkanDescriptorLayout layout{};

		List<ShaderUniformDescriptor> uniforms = _subshader.GetScopedUniforms(scope);
		List<ShaderTextureSampler> samplers = _subshader.GetScopedSamplers(scope);

		// Create subshader descriptor layout
		layout.LayoutBindings.Resize(samplers.Count() + (uniforms.Count() > 0 ? 1 : 0));
		uint32_t bindingIndex = 0;

		if (uniforms.Count() > 0)
		{
			VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings[bindingIndex];
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlagBits(_subshader.GetUniformBindingStages(scope));

			bindingIndex++;
		}

		for (uint32_t i = 0; i < samplers.Count(); i++)
		{
			VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings[bindingIndex];
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlagBits(samplers[i].BindingPoints);

			bindingIndex++;
		}

		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(layout.LayoutBindings.Count());
		createInfo.pBindings = layout.LayoutBindings.Data();

		AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &layout.Layout));

		return layout;
	}

	VulkanShader::VulkanShader(const ResourceID& id, const string& name, const ShaderRenderData& shaderData) :
		GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name), 
		CachedResource(shaderData.ID, shaderData.Version)
	{}

	bool VulkanShader::NeedsUpdate(const ShaderRenderData& shaderData) const noexcept
	{
		return GetReferenceVersion() != shaderData.Version || _subshaders.size() == 0 ||
			std::any_of(_subshaders.cbegin(), _subshaders.cend(), [](const auto& kvp) {
				return kvp.second._shaderStages.Count() == 0 || kvp.second._descriptorLayouts.size() == 0;
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
		{
			for(const auto& layout : subshader.second.GetDescriptorLayouts())
				layouts.Add(layout);
		}

		return layouts;
	}
}
