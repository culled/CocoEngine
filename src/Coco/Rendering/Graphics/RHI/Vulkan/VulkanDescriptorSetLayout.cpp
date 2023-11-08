#include "Renderpch.h"
#include "VulkanDescriptorSetLayout.h"

#include "VulkanUtils.h"

#include "VulkanGraphicsDevice.h"

namespace Coco::Rendering::Vulkan
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(uint64 layoutHash) :
		LayoutHash(layoutHash),
		Layout(nullptr),
		LayoutBindings{},
		DataSize(0)
	{}

	VulkanDescriptorSetLayout VulkanDescriptorSetLayout::CreateForUniformLayout(VulkanGraphicsDevice& device, const ShaderUniformLayout& layout, bool includeDataUniforms)
	{
		VulkanDescriptorSetLayout setLayout(layout.Hash);

		bool hasDataUniforms = layout.HasDataUniforms() && includeDataUniforms;

		uint32 bindingIndex = 0;

		if (hasDataUniforms)
		{
			VkDescriptorSetLayoutBinding& binding = setLayout.LayoutBindings.emplace_back();
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(layout.GetUniformBindStages(true, false));

			VkWriteDescriptorSet& write = setLayout.WriteTemplates.emplace_back();
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			setLayout.DataSize = layout.GetUniformDataSize(device);

			bindingIndex++;
		}

		for (const ShaderUniform& uniform : layout.Uniforms)
		{
			if (uniform.Type != ShaderUniformType::Texture)
				continue;

			VkDescriptorSetLayoutBinding& binding = setLayout.LayoutBindings.emplace_back();
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.pImmutableSamplers = nullptr;
			binding.binding = bindingIndex;
			binding.stageFlags = ToVkShaderStageFlags(uniform.BindingPoints);

			VkWriteDescriptorSet& write = setLayout.WriteTemplates.emplace_back();
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;

			bindingIndex++;
		}

		if (const GlobalShaderUniformLayout* globalLayout = dynamic_cast<const GlobalShaderUniformLayout*>(&layout))
		{
			for (const ShaderBufferUniform& uniform : globalLayout->BufferUniforms)
			{
				VkDescriptorSetLayoutBinding& binding = setLayout.LayoutBindings.emplace_back();
				binding.descriptorCount = 1;
				binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				binding.pImmutableSamplers = nullptr;
				binding.binding = bindingIndex;
				binding.stageFlags = ToVkShaderStageFlags(uniform.BindingPoints);

				VkWriteDescriptorSet& write = setLayout.WriteTemplates.emplace_back();
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.dstBinding = bindingIndex;
				write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				write.descriptorCount = 1;

				bindingIndex++;
			}
		}

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(setLayout.LayoutBindings.size());
		createInfo.pBindings = setLayout.LayoutBindings.size() > 0 ? setLayout.LayoutBindings.data() : nullptr;
		
		AssertVkSuccess(vkCreateDescriptorSetLayout(device.GetDevice(), &createInfo, device.GetAllocationCallbacks(), &setLayout.Layout))

		return setLayout;
	}

	uint32 VulkanDescriptorSetLayout::GetTypeCount(VkDescriptorType descriptorType) const
	{
		uint32 count = 0;

		for (const VkDescriptorSetLayoutBinding& binding : LayoutBindings)
			if (binding.descriptorType == descriptorType)
				count++;

		return count;
	}
}