#include "Renderpch.h"
#include "VulkanShader.h"

#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"
#include "../VulkanShaderCache.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanShaderStage::VulkanShaderStage(const ShaderStage& stage) :
		ShaderStage(stage),
		ShaderModule(nullptr),
		ShaderModuleCreateInfo{}
	{}

	VulkanShader::VulkanShader(const SharedRef<Shader>& shader) :
		CachedVulkanResource(MakeKey(shader)),
		_version(0),
		_shader(shader),
		_stages{},
		_layouts{}
	{}

	VulkanShader::~VulkanShader()
	{
		DestroyShaderObjects();
	}

	GraphicsDeviceResourceID VulkanShader::MakeKey(const SharedRef<Shader>& shader)
	{
		std::hash<string> strHasher;
		return strHasher(shader->GetName());
	}

	const VulkanDescriptorSetLayout& VulkanShader::GetDescriptorSetLayout(UniformScope scope) const
	{
		Assert(_layouts.contains(scope))

		return _layouts.at(scope);
	}

	bool VulkanShader::HasScope(UniformScope scope) const
	{
		// A layout will not be generated for data-only uniforms in the draw scope, so just test if the layout is not empty for the behavior for the draw scope
		if (scope == UniformScope::Draw)
		{
			return _shader->GetDrawUniformLayout().Hash != ShaderUniformLayout::EmptyHash;
		}

		return _layouts.contains(scope);
	}

	std::vector<VkPushConstantRange> VulkanShader::GetPushConstantRanges() const
	{
		const uint64 dataSize = _shader->GetDrawUniformLayout().GetUniformDataSize(_device);

		if (dataSize == 0)
			return std::vector<VkPushConstantRange>();

		const VulkanGraphicsDeviceFeatures& features = _device.GetVulkanFeatures();

		if (dataSize > features.MaxPushConstantSize)
		{
			CocoError("Cannot have a push constant buffer greater than the maximum supported {} bytes. Requested: {} bytes", features.MaxPushConstantSize, dataSize)
			return std::vector<VkPushConstantRange>();
		}

		//const std::vector<ShaderDataUniform>& drawUniforms = _variant.DrawUniforms.DataUniforms;

		VkPushConstantRange range{};
		range.offset = 0;
		range.size = static_cast<uint32_t>(dataSize);
		range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // TODO: configurable push constant stages

		/*uint64 offset = 0;
		
		for (int i = 0; i < drawUniforms.size(); i++)
		{
			const ShaderDataUniform& uniform = drawUniforms.at(i);
			const uint32 size = GetBufferDataTypeSize(uniform.Type);

			_device.AlignOffset(uniform.Type, offset);

			range.size += size;

			offset += size;
		}*/

		return std::vector<VkPushConstantRange>({ range });
	}

	bool VulkanShader::NeedsUpdate() const
	{
		return _shader->GetVersion() != _version || _stages.size() == 0;
	}

	void VulkanShader::Update()
	{
		DestroyShaderObjects();
		CreateShaderObjects();

		_version = _shader->GetVersion();
	}

	void VulkanShader::CreateShaderObjects()
	{
		for (const ShaderStage& stage : _shader->GetStages())
		{
			CreateStage(stage);
		}

		const auto& globalLayout = _shader->GetGlobalUniformLayout();
		if (globalLayout.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(globalLayout, UniformScope::ShaderGlobal);

		const auto& instanceLayout = _shader->GetInstanceUniformLayout();
		if (instanceLayout.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(instanceLayout, UniformScope::Instance);

		const auto& drawLayout = _shader->GetDrawUniformLayout();
		if (drawLayout.HasTextureUniforms())
			CreateLayout(drawLayout, UniformScope::Draw);

		CocoTrace("Created VulkanShader")
	}

	void VulkanShader::DestroyShaderObjects()
	{
		if (_layouts.size() == 0 && _stages.size() == 0)
			return;

		for (const auto& kvp : _layouts)
			DestroyLayout(kvp.first);

		_layouts.clear();

		for (const VulkanShaderStage& stage : _stages)
			DestroyShaderStage(stage);

		_stages.clear();

		CocoTrace("Destroyed VulkanShader")
	}

	void VulkanShader::CreateStage(const ShaderStage& stage)
	{
		VulkanShaderStage vulkanStage(stage);
		VkShaderModuleCreateInfo& createInfo = vulkanStage.ShaderModuleCreateInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		
		VulkanShaderCache& cache = static_cast<VulkanShaderCache&>(_device.GetShaderCache());
		std::vector<uint32> byteCode = cache.CompileOrGetShaderStageBinary(vulkanStage);

		createInfo.codeSize = byteCode.size() * sizeof(uint32);
		createInfo.pCode = byteCode.data();

		AssertVkSuccess(vkCreateShaderModule(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &vulkanStage.ShaderModule))

		_stages.push_back(vulkanStage);
		CocoTrace("Created VulkanShaderStage for file {}", stage.CompiledFilePath.ToString())
	}

	void VulkanShader::DestroyShaderStage(const VulkanShaderStage& stage)
	{
		if (!stage.ShaderModule)
			return;

		_device.WaitForIdle();
		vkDestroyShaderModule(_device.GetDevice(), stage.ShaderModule, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanShaderStage")
	}

	void VulkanShader::CreateLayout(const ShaderUniformLayout& layout, UniformScope scope)
	{
		_layouts.try_emplace(scope, VulkanDescriptorSetLayout::CreateForUniformLayout(_device, layout, scope != UniformScope::Draw));

		CocoTrace("Created VulkanDescriptorSetLayout for scope {}", static_cast<int>(scope))
	}

	void VulkanShader::DestroyLayout(UniformScope scope)
	{
		auto it = _layouts.find(scope);

		if (it == _layouts.end() || !it->second.Layout)
			return;

		_device.WaitForIdle();
		vkDestroyDescriptorSetLayout(_device.GetDevice(), it->second.Layout, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanDescriptorSetLayout for scope {}", static_cast<int>(it->first))
	}
}