#include "Renderpch.h"
#include "VulkanDescriptorSetLayout.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanGraphicsDevice& device, const ShaderUniformLayout& layout, bool includeDataUniforms) :
		CachedVulkanResource(MakeKey(layout, includeDataUniforms)),
		_device(device),
		_uniformLayout(layout),
		_includesDataUniforms(includeDataUniforms),
		_layoutBindings()
	{
		if (_uniformLayout.NeedsDataCalculation())
			_uniformLayout.CalculateDataUniforms(_device);

		bool hasDataUniforms = layout.HasUniformsOfType(true, false) && includeDataUniforms;

		uint32 bindingIndex = 0;

		if (hasDataUniforms)
		{
			VkDescriptorSetLayoutBinding& binding = _layoutBindings.emplace_back();
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(layout.GetUniformBindStages(true, false));

			bindingIndex++;
		}

		for (const ShaderUniform* uniform : _uniformLayout.GetUniforms(false, true))
		{
			VkDescriptorSetLayoutBinding& binding = _layoutBindings.emplace_back();
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(uniform->BindingPoints);

			bindingIndex++;
		}

		VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		createInfo.bindingCount = static_cast<uint32_t>(_layoutBindings.size());
		createInfo.pBindings = _layoutBindings.data();

		AssertVkSuccess(vkCreateDescriptorSetLayout(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_layout))

		_descriptorSetPool = CreateUniqueRef<VulkanDescriptorSetPool>(_device, *this);

		CocoTrace("Created VulkanDescriptorSetLayout {} for layout {} - include data uniforms: {}", ID, layout.GetHash(), _includesDataUniforms)
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		if (_layout)
		{
			_device.WaitForIdle();
			vkDestroyDescriptorSetLayout(_device.GetDevice(), _layout, _device.GetAllocationCallbacks());
			_layout = nullptr;
		}

		_layoutBindings.clear();

		CocoTrace("Destroyed VulkanDescriptorSetLayout {}", ID)
	}

	uint64 VulkanDescriptorSetLayout::MakeKey(const ShaderUniformLayout& layout, bool includeDataUniforms)
	{
		return Math::CombineHashes(includeDataUniforms ? 1 : 0, layout.GetHash());
	}

	std::vector<VkWriteDescriptorSet> VulkanDescriptorSetLayout::GetDescriptorWrites() const
	{
		std::vector<VkWriteDescriptorSet> writes;
		writes.reserve(_layoutBindings.size());

		for (const VkDescriptorSetLayoutBinding& binding : _layoutBindings)
		{
			VkWriteDescriptorSet& write = writes.emplace_back(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
			write.descriptorType = binding.descriptorType;
			write.descriptorCount = binding.descriptorCount;
			write.dstBinding = binding.binding;
		}

		return writes;
	}

	bool VulkanDescriptorSetLayout::HasUniformsOfType(bool dataUniforms, bool textureUniforms) const
	{
		return _uniformLayout.HasUniformsOfType(dataUniforms && _includesDataUniforms, textureUniforms);
	}
}