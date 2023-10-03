#include "Renderpch.h"
#include "VulkanShaderVariant.h"

#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanShaderStage::VulkanShaderStage(const ShaderStage& stage) :
		ShaderStage(stage),
		ShaderModule(nullptr),
		ShaderModuleCreateInfo{}
	{}

	VulkanShaderVariant::VulkanShaderVariant(const ShaderVariantData& variantData) :
		CachedVulkanResource(MakeKey(variantData)),
		_version(0),
		_variant(variantData.Variant),
		_stages{},
		_layouts{}
	{}

	VulkanShaderVariant::~VulkanShaderVariant()
	{
		DestroyShaderObjects();
	}

	GraphicsDeviceResourceID VulkanShaderVariant::MakeKey(const ShaderVariantData& variantData)
	{
		return variantData.ID;
	}

	std::vector<VulkanDescriptorSetLayout> VulkanShaderVariant::GetDescriptorSetLayouts() const
	{
		std::vector<VulkanDescriptorSetLayout> layouts;

		for (const auto& kvp : _layouts)
			layouts.push_back(kvp.second);

		return layouts;
	}

	const VulkanDescriptorSetLayout& VulkanShaderVariant::GetDescriptorSetLayout(UniformScope scope) const
	{
		Assert(_layouts.contains(scope))

		return _layouts.at(scope);
	}

	bool VulkanShaderVariant::HasScope(UniformScope scope) const
	{
		// A layout will not be generated for data-only uniforms in the draw scope, so just test if the layout is not empty for the behavior for the draw scope
		if (scope == UniformScope::Draw && _variant.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash)
			return true;

		return _layouts.contains(scope);
	}

	std::vector<VkPushConstantRange> VulkanShaderVariant::GetPushConstantRanges() const
	{
		const uint64 dataSize = _variant.DrawUniforms.GetUniformDataSize(_device);

		if (dataSize == 0)
			return std::vector<VkPushConstantRange>();

		const VulkanGraphicsDeviceFeatures& features = _device.GetVulkanFeatures();

		if (dataSize > features.MaxPushConstantSize)
		{
			CocoError("Cannot have a push constant buffer greater than the maximum supported {} bytes. Requested: {} bytes", features.MaxPushConstantSize, dataSize)
			return std::vector<VkPushConstantRange>();
		}

		const std::vector<ShaderDataUniform>& drawUniforms = _variant.DrawUniforms.DataUniforms;

		VkPushConstantRange range{};
		range.offset = 0;
		range.size = static_cast<uint32_t>(dataSize);
		range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // TODO: configurable push constant stages

		/*uint64 offset = 0;
		
		for (int i = 0; i < drawUniforms.size(); i++)
		{
			const ShaderDataUniform& uniform = drawUniforms.at(i);
			const uint32 size = GetDataTypeSize(uniform.Type);

			_device.AlignOffset(uniform.Type, offset);

			range.size += size;

			offset += size;
		}*/

		return std::vector<VkPushConstantRange>({ range });
	}

	bool VulkanShaderVariant::NeedsUpdate(const ShaderVariantData& variantData) const
	{
		return variantData.Version != _version ||
			_stages.size() == 0;
	}

	void VulkanShaderVariant::Update(const ShaderVariantData& variantData)
	{
		_variant = variantData.Variant;

		DestroyShaderObjects();
		CreateShaderObjects();

		_version = variantData.Version;
	}

	void VulkanShaderVariant::CreateShaderObjects()
	{
		for (const ShaderStage& stage : _variant.Stages)
		{
			CreateStage(stage);
		}

		if (_variant.GlobalUniforms.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(UniformScope::ShaderGlobal);

		if (_variant.InstanceUniforms.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(UniformScope::Instance);

		if (_variant.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash && _variant.DrawUniforms.TextureUniforms.size() > 0)
			CreateLayout(UniformScope::Draw);

		CocoTrace("Created VulkanShaderVariant")
	}

	void VulkanShaderVariant::DestroyShaderObjects()
	{
		if (_layouts.size() == 0 && _stages.size() == 0)
			return;

		for (const auto& kvp : _layouts)
			DestroyLayout(kvp.first);

		_layouts.clear();

		for (const VulkanShaderStage& stage : _stages)
			DestroyShaderStage(stage);

		_stages.clear();

		CocoTrace("Destroyed VulkanShaderVariant")
	}

	void VulkanShaderVariant::CreateStage(const ShaderStage& stage)
	{
		VulkanShaderStage vulkanStage(stage);
		VkShaderModuleCreateInfo& createInfo = vulkanStage.ShaderModuleCreateInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		File shaderFile = Engine::Get()->GetFileSystem().OpenFile(stage.FilePath, FileOpenFlags::Read);
		std::vector<uint8> byteCode = shaderFile.ReadToEnd();
		shaderFile.Close();

		createInfo.codeSize = byteCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.data());

		AssertVkSuccess(vkCreateShaderModule(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &vulkanStage.ShaderModule))

		_stages.push_back(vulkanStage);
		CocoTrace("Created VulkanShaderStage for file {}", stage.FilePath)
	}

	void VulkanShaderVariant::DestroyShaderStage(const VulkanShaderStage& stage)
	{
		if (!stage.ShaderModule)
			return;

		_device.WaitForIdle();
		vkDestroyShaderModule(_device.GetDevice(), stage.ShaderModule, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanShaderStage")
	}

	void VulkanShaderVariant::CreateLayout(UniformScope scope)
	{
		const ShaderUniformLayout& layout = scope == UniformScope::ShaderGlobal ? _variant.GlobalUniforms : 
			(scope == UniformScope::Instance ? _variant.InstanceUniforms : _variant.DrawUniforms);
		
		_layouts.try_emplace(scope, VulkanDescriptorSetLayout::CreateForUniformLayout(_device, layout, scope != UniformScope::Draw));

		CocoTrace("Created VulkanDescriptorSetLayout for scope {}", static_cast<int>(scope))
	}

	void VulkanShaderVariant::DestroyLayout(UniformScope scope)
	{
		auto it = _layouts.find(scope);

		if (it == _layouts.end() || !it->second.Layout)
			return;

		_device.WaitForIdle();
		vkDestroyDescriptorSetLayout(_device.GetDevice(), it->second.Layout, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanDescriptorSetLayout for scope {}", static_cast<int>(it->first))
	}
}