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

	VulkanRenderPassShader::VulkanRenderPassShader(const RenderPassShader& shaderInfo) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(MakeKey(shaderInfo)),
		_version(0),
		_shaderInfo(shaderInfo),
		_stages{},
		_layouts{},
		_lastUsedTime(0.0)
	{}

	VulkanRenderPassShader::~VulkanRenderPassShader()
	{
		DestroyShaderObjects();
	}

	GraphicsDeviceResourceID VulkanRenderPassShader::MakeKey(const RenderPassShader& shaderInfo)
	{
		return shaderInfo.ID;
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

	uint32 VulkanRenderPassShader::GetUniformDataSize(UniformScope scope) const
	{
		uint64 offset = 0;

		for (const auto& u : _shaderInfo.GetScopedDataUniforms(scope))
		{
			_device.AlignOffset(u.Type, offset);
			offset += GetDataTypeSize(u.Type);
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		offset = GraphicsDevice::GetOffsetForAlignment(offset, _device.GetMinimumBufferAlignment());

		return static_cast<uint32>(offset);
	}

	std::vector<VkPushConstantRange> VulkanRenderPassShader::GetPushConstantRanges() const
	{
		const uint32 dataSize = GetUniformDataSize(UniformScope::Draw);

		if (dataSize == 0)
			return std::vector<VkPushConstantRange>();

		// TODO: check maximum push constant range size
		if (dataSize > 128)
		{
			CocoError("Cannot have a push constant buffer greater than 128 bytes. Requested: {} bytes", dataSize)
			return std::vector<VkPushConstantRange>();
		}

		std::vector<ShaderDataUniform> drawUniforms = _shaderInfo.GetScopedDataUniforms(UniformScope::Draw);

		VkPushConstantRange range{};
		range.offset = 0;
		range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // TODO: configurable push constant stages

		uint64 offset = 0;

		for (int i = 0; i < drawUniforms.size(); i++)
		{
			const ShaderDataUniform& uniform = drawUniforms.at(i);
			const uint32 size = GetDataTypeSize(uniform.Type);

			_device.AlignOffset(uniform.Type, offset);

			range.size += size;

			offset += size;
		}

		return std::vector<VkPushConstantRange>({ range });
	}

	bool VulkanRenderPassShader::NeedsUpdate(const RenderPassShader& shaderInfo) const
	{
		return shaderInfo.Version != _version || _stages.size() == 0;
	}

	void VulkanRenderPassShader::Update(const RenderPassShader& shaderInfo)
	{
		DestroyShaderObjects();
		CreateShaderObjects(shaderInfo);

		_version = shaderInfo.Version;
	}

	void VulkanRenderPassShader::Use()
	{
		_lastUsedTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanRenderPassShader::IsStale() const
	{
		double currentTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanGraphicsDeviceCache::sPurgeThreshold;
	}

	void VulkanRenderPassShader::CreateShaderObjects(const RenderPassShader& shaderInfo)
	{
		for (const ShaderStage& stage : _shaderInfo.Stages)
		{
			CreateStage(stage);
		}

		if (shaderInfo.HasScope(UniformScope::Global))
			CreateLayout(UniformScope::Global);

		if (shaderInfo.HasScope(UniformScope::Instance))
			CreateLayout(UniformScope::Instance);

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
		VulkanDescriptorSetLayout layout{};

		std::vector<ShaderDataUniform> dataUniforms = _shaderInfo.GetScopedDataUniforms(scope);
		std::vector<ShaderTextureUniform> textureUniforms = _shaderInfo.GetScopedTextureUniforms(scope);

		bool hasDataUniforms = dataUniforms.size() > 0;

		layout.LayoutBindings.resize(textureUniforms.size() + (hasDataUniforms ? 1 : 0));
		uint32 bindingIndex = 0;

		if (hasDataUniforms)
		{
			VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings.at(bindingIndex);
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(_shaderInfo.GetUniformScopeBindStages(scope));

			bindingIndex++;
		}

		for (uint32 i = 0; i < textureUniforms.size(); i++)
		{
			VkDescriptorSetLayoutBinding& binding = layout.LayoutBindings.at(bindingIndex);
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(textureUniforms.at(i).BindingPoints);

			bindingIndex++;
		}

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(layout.LayoutBindings.size());
		createInfo.pBindings = layout.LayoutBindings.data();

		AssertVkSuccess(vkCreateDescriptorSetLayout(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &layout.Layout));

		_layouts.try_emplace(scope, layout);

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