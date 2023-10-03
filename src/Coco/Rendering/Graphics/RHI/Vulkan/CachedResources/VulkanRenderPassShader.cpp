#include "Renderpch.h"
#include "VulkanRenderPassShader.h"

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

	VulkanRenderPassShader::VulkanRenderPassShader(const RenderPassShaderData& shader) :
		CachedVulkanResource(MakeKey(shader)),
		_version(0),
		_shaderInfo(shader.ShaderData),
		_stages{},
		_layouts{}
	{}

	VulkanRenderPassShader::~VulkanRenderPassShader()
	{
		DestroyShaderObjects();
	}

	GraphicsDeviceResourceID VulkanRenderPassShader::MakeKey(const RenderPassShaderData& shader)
	{
		return shader.ID;
	}

	std::vector<VulkanDescriptorSetLayout> VulkanRenderPassShader::GetDescriptorSetLayouts() const
	{
		std::vector<VulkanDescriptorSetLayout> layouts;

		for (const auto& kvp : _layouts)
			layouts.push_back(kvp.second);

		return layouts;
	}

	const VulkanDescriptorSetLayout& VulkanRenderPassShader::GetDescriptorSetLayout(UniformScope scope) const
	{
		Assert(_layouts.contains(scope))

		return _layouts.at(scope);
	}

	bool VulkanRenderPassShader::HasScope(UniformScope scope) const
	{
		// A layout will not be generated for data-only uniforms in the draw scope, so just test if the layout is not empty for the behavior for the draw scope
		if (scope == UniformScope::Draw && _shaderInfo.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash)
			return true;

		return _layouts.contains(scope);
	}

	std::vector<VkPushConstantRange> VulkanRenderPassShader::GetPushConstantRanges() const
	{
		const uint64 dataSize = _shaderInfo.DrawUniforms.GetUniformDataSize(_device);

		if (dataSize == 0)
			return std::vector<VkPushConstantRange>();

		const VulkanGraphicsDeviceFeatures& features = _device.GetVulkanFeatures();

		if (dataSize > features.MaxPushConstantSize)
		{
			CocoError("Cannot have a push constant buffer greater than the maximum supported {} bytes. Requested: {} bytes", features.MaxPushConstantSize, dataSize)
			return std::vector<VkPushConstantRange>();
		}

		const std::vector<ShaderDataUniform>& drawUniforms = _shaderInfo.DrawUniforms.DataUniforms;

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

	bool VulkanRenderPassShader::NeedsUpdate(const RenderPassShaderData& shaderInfo) const
	{
		return shaderInfo.Version != _version || 
			_stages.size() == 0;
	}

	void VulkanRenderPassShader::Update(const RenderPassShaderData& shaderInfo)
	{
		DestroyShaderObjects();
		CreateShaderObjects(shaderInfo.ShaderData);

		_version = shaderInfo.Version;
	}

	void VulkanRenderPassShader::CreateShaderObjects(const RenderPassShader& shaderInfo)
	{
		for (const ShaderStage& stage : _shaderInfo.Stages)
		{
			CreateStage(stage);
		}

		if (shaderInfo.GlobalUniforms.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(UniformScope::ShaderGlobal);

		if (shaderInfo.InstanceUniforms.Hash != ShaderUniformLayout::EmptyHash)
			CreateLayout(UniformScope::Instance);

		if (shaderInfo.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash && shaderInfo.DrawUniforms.TextureUniforms.size() > 0)
			CreateLayout(UniformScope::Draw);

		CocoTrace("Created VulkanRenderPassShader")
	}

	void VulkanRenderPassShader::DestroyShaderObjects()
	{
		if (_layouts.size() == 0 && _stages.size() == 0)
			return;

		for (const auto& kvp : _layouts)
			DestroyLayout(kvp.first);

		_layouts.clear();

		for (const VulkanShaderStage& stage : _stages)
			DestroyShaderStage(stage);

		_stages.clear();

		CocoTrace("Destroyed VulkanRenderPassShader")
	}

	void VulkanRenderPassShader::CreateStage(const ShaderStage& stage)
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

	void VulkanRenderPassShader::DestroyShaderStage(const VulkanShaderStage& stage)
	{
		if (!stage.ShaderModule)
			return;

		_device.WaitForIdle();
		vkDestroyShaderModule(_device.GetDevice(), stage.ShaderModule, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanShaderStage")
	}

	void VulkanRenderPassShader::CreateLayout(UniformScope scope)
	{
		const ShaderUniformLayout& layout = scope == UniformScope::ShaderGlobal ? _shaderInfo.GlobalUniforms : 
			(scope == UniformScope::Instance ? _shaderInfo.InstanceUniforms : _shaderInfo.DrawUniforms);
		
		_layouts.try_emplace(scope, VulkanDescriptorSetLayout::CreateForUniformLayout(_device, layout, scope != UniformScope::Draw));

		CocoTrace("Created VulkanDescriptorSetLayout for scope {}", static_cast<int>(scope))
	}

	void VulkanRenderPassShader::DestroyLayout(UniformScope scope)
	{
		auto it = _layouts.find(scope);

		if (it == _layouts.end() || !it->second.Layout)
			return;

		_device.WaitForIdle();
		vkDestroyDescriptorSetLayout(_device.GetDevice(), it->second.Layout, _device.GetAllocationCallbacks());

		CocoTrace("Destroyed VulkanDescriptorSetLayout for scope {}", static_cast<int>(it->first))
	}
}