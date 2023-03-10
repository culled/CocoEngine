#include "DescriptorPoolVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"

namespace Coco::Rendering
{
	DescriptorPoolVulkan::DescriptorPoolVulkan(GraphicsDevice* owningDevice, uint maxSets, const List<VulkanDescriptorLayout>& descriptorSetLayouts) :
		_device(static_cast<GraphicsDeviceVulkan*>(owningDevice)),
		_maxDescriptorSets(maxSets), _descriptorSetLayouts(descriptorSetLayouts)
	{
		List<VkDescriptorPoolSize> poolSizes;

		for (const VulkanDescriptorLayout& layout : _descriptorSetLayouts)
		{
			for (const VkDescriptorSetLayoutBinding& binding : layout.LayoutBindings)
			{
				VkDescriptorPoolSize poolSize = {};
				poolSize.type = binding.descriptorType;
				poolSize.descriptorCount = layout.GetTypeCount(binding.descriptorType);

				poolSizes.Add(poolSize);
			}
		}

		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.Count());
		poolCreateInfo.pPoolSizes = poolSizes.Data();
		poolCreateInfo.maxSets = maxSets;

		AssertVkResult(vkCreateDescriptorPool(_device->GetDevice(), &poolCreateInfo, nullptr, &_pool));
	}

	DescriptorPoolVulkan::~DescriptorPoolVulkan()
	{
		try
		{
			FreeSets();
		}
		catch(...)
		{ }

		if (_pool != nullptr)
		{
			vkDestroyDescriptorPool(_device->GetDevice(), _pool, nullptr);
			_pool = nullptr;
		}
	}

	VkDescriptorSet DescriptorPoolVulkan::GetOrAllocateSet(const VulkanDescriptorLayout& layout, uint64_t key)
	{
		if (_allocatedDescriptorSets.contains(key))
			return _allocatedDescriptorSets[key];

		if (_allocatedDescriptorSets.size() >= _maxDescriptorSets)
			throw Exception("Already allocated the maximum number of descriptor sets");

		VkDescriptorSetAllocateInfo setAllocate = {};
		setAllocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocate.descriptorPool = _pool;
		setAllocate.descriptorSetCount = 1;
		setAllocate.pSetLayouts = &layout.Layout;

		VkDescriptorSet set;
		AssertVkResult(vkAllocateDescriptorSets(_device->GetDevice(), &setAllocate, &set));
		
		_allocatedDescriptorSets[key] = set;

		return set;
	}

	void DescriptorPoolVulkan::FreeSets() noexcept
	{
		try
		{
			AssertVkResult(vkResetDescriptorPool(_device->GetDevice(), _pool, 0));
			_allocatedDescriptorSets.clear();
		}
		catch (const Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to free descriptor sets: {}", ex.what()));
		}
	}
}
