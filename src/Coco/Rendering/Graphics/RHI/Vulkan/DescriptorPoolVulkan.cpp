#include "DescriptorPoolVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"

namespace Coco::Rendering
{
	DescriptorPoolVulkan::DescriptorPoolVulkan(GraphicsDevice* owningDevice, uint maxSets, const List<VkDescriptorSetLayout>& descriptorSetLayouts) :
		_device(static_cast<GraphicsDeviceVulkan*>(owningDevice)), 
		_descriptorSetLayouts(descriptorSetLayouts),
		_maxDescriptorSets(maxSets)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = maxSets;

		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = 1;
		poolCreateInfo.pPoolSizes = &poolSize;
		poolCreateInfo.maxSets = maxSets;

		AssertVkResult(vkCreateDescriptorPool(_device->GetDevice(), &poolCreateInfo, nullptr, &_pool));
	}

	DescriptorPoolVulkan::~DescriptorPoolVulkan()
	{

		//Map<uint64_t, VkDescriptorSet> setsToFree(_allocatedDescriptorSets);
		//
		//for (const auto& kvp : setsToFree)
		//{
		//	FreeSet(kvp.first);
		//}
		//
		//if (setsToFree.size() > 0)
		//	LogTrace(_device->VulkanPlatform->GetLogger(), FormattedString("Freeing {} descriptor sets", setsToFree.size()));

		_allocatedDescriptorSets.clear();

		if (_pool != nullptr)
		{
			vkResetDescriptorPool(_device->GetDevice(), _pool, 0);
			vkDestroyDescriptorPool(_device->GetDevice(), _pool, nullptr);
			_pool = nullptr;
		}
	}

	VkDescriptorSet DescriptorPoolVulkan::GetOrAllocateSet(uint64_t key)
	{
		if (_allocatedDescriptorSets.contains(key))
			return _allocatedDescriptorSets[key];

		if (_allocatedDescriptorSets.size() >= _maxDescriptorSets)
			throw Exception("Already allocated the maximum number of descriptor sets");

		VkDescriptorSetAllocateInfo setAllocate = {};
		setAllocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		setAllocate.descriptorPool = _pool;
		setAllocate.descriptorSetCount = static_cast<uint32_t>(_descriptorSetLayouts.Count());
		setAllocate.pSetLayouts = _descriptorSetLayouts.Data();

		VkDescriptorSet set;
		AssertVkResult(vkAllocateDescriptorSets(_device->GetDevice(), &setAllocate, &set));
		
		_allocatedDescriptorSets[key] = set;

		return set;
	}

	//void DescriptorPoolVulkan::FreeSet(uint64_t key)
	//{
	//	if (!_allocatedDescriptorSets.contains(key))
	//	{
	//		LogWarning(_device->VulkanPlatform->GetLogger(), FormattedString("Tried to free a descriptor set for invalid key {}", key));
	//		return;
	//	}
	//	
	//	vkFreeDescriptorSets(_device->GetDevice(), _pool, 1, &_allocatedDescriptorSets[key]);
	//	
	//	_allocatedDescriptorSets.erase(key);
	//}
}
