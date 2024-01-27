#include "Renderpch.h"
#include "VulkanDescriptorSetPool.h"
#include "../VulkanGraphicsDevice.h"
#include "../CachedResources/VulkanDescriptorSetLayout.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	AllocatedVulkanDescriptorPool::AllocatedVulkanDescriptorPool() :
		Pool(nullptr),
		LastAllocateTime(0.0)
	{}

	const uint32 VulkanDescriptorSetPool::_maxSets = 512;

	VulkanDescriptorSetPool::VulkanDescriptorSetPool(VulkanGraphicsDevice& device, const VulkanDescriptorSetLayout& layout) :
		_device(device),
		_layoutID(layout.ID),
		_createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
		_poolSizes(),
		_pools(),
		_allocatedSets(),
		_setLayout(layout.GetLayout())
	{
		std::unordered_map<VkDescriptorType, uint32> typeCounts = GetTypeCounts(layout);

		for (const VkDescriptorSetLayoutBinding& binding : layout.GetBindings())
		{
			VkDescriptorPoolSize& poolSize = _poolSizes.emplace_back();
			poolSize.type = binding.descriptorType;
			poolSize.descriptorCount = typeCounts[binding.descriptorType] * _maxSets;
		}

		_createInfo.poolSizeCount = static_cast<uint32>(_poolSizes.size());
		_createInfo.pPoolSizes = _poolSizes.data();
		_createInfo.maxSets = _maxSets;

		CreateDescriptorPool();
	}

	VulkanDescriptorSetPool::~VulkanDescriptorSetPool()
	{
		DestroyDescriptorPools();
	}

	bool VulkanDescriptorSetPool::TryGetAllocatedSet(uint64 setKey, VkDescriptorSet& outSet) const
	{
		auto it = _allocatedSets.find(setKey);
		if (it == _allocatedSets.end())
			return false;

		outSet = it->second;
		return true;
	}

	VkDescriptorSet VulkanDescriptorSetPool::AllocateDescriptorSet(uint64 setKey)
	{
		auto setIt = _allocatedSets.find(setKey);
		if (setIt != _allocatedSets.end())
			return setIt->second;

		auto poolIt = _pools.begin();
		VkDescriptorSet set = nullptr;

		VkDescriptorSetAllocateInfo setAllocate{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		setAllocate.descriptorSetCount = 1;
		setAllocate.pSetLayouts = &_setLayout;

		do
		{
			setAllocate.descriptorPool = poolIt->Pool;

			VkResult poolResult = vkAllocateDescriptorSets(_device.GetDevice(), &setAllocate, &set);

			if (poolResult == VK_SUCCESS)
			{
				poolIt->LastAllocateTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
				break;
			}

			if (poolResult == VK_ERROR_OUT_OF_POOL_MEMORY)
			{
				++poolIt;

				if (poolIt == _pools.end())
				{
					CreateDescriptorPool();
					poolIt = _pools.end();
					--poolIt;
				}
			}
			else
			{
				AssertVkSuccess(poolResult)
			}
		} while (!set);

		_allocatedSets.try_emplace(setKey, set);
		return set;
	}

	uint64 VulkanDescriptorSetPool::PurgeUnusedPools(double staleThreshold)
	{
		double time = MainLoop::Get()->GetCurrentTick().UnscaledTime;

		uint64 purged = 0;
		auto it = _pools.begin();
		while (it != _pools.end())
		{
			AllocatedVulkanDescriptorPool& pool = *it;

			if (time - pool.LastAllocateTime > staleThreshold)
			{
				DestroyDescriptorPool(it);
				purged++;
			}
			else
			{
				++it;
			}
		}

		if (purged > 0)
		{
			CocoTrace("Purged {} VulkanDescriptorPools", purged)
		}

		return purged;
	}

	void VulkanDescriptorSetPool::FreeSets()
	{
		_allocatedSets.clear();

		for (const AllocatedVulkanDescriptorPool& pool : _pools)
			AssertVkSuccess(vkResetDescriptorPool(_device.GetDevice(), pool.Pool, 0))
	}

	std::unordered_map<VkDescriptorType, uint32> VulkanDescriptorSetPool::GetTypeCounts(const VulkanDescriptorSetLayout& layout)
	{
		std::unordered_map<VkDescriptorType, uint32> counts;

		for (const VkDescriptorSetLayoutBinding& binding : layout.GetBindings())
		{
			counts[binding.descriptorType] += binding.descriptorCount;
		}

		return counts;
	}

	AllocatedVulkanDescriptorPool& VulkanDescriptorSetPool::CreateDescriptorPool()
	{
		AllocatedVulkanDescriptorPool& pool = _pools.emplace_back();
		AssertVkSuccess(vkCreateDescriptorPool(_device.GetDevice(), &_createInfo, _device.GetAllocationCallbacks(), &pool.Pool));

		CocoTrace("Created VulkanDescriptorPool for layout ID {}. Pool count is now {}", _layoutID, _pools.size())
		return pool;
	}

	void VulkanDescriptorSetPool::DestroyDescriptorPool(PoolList::iterator& poolIt)
	{
		_device.WaitForIdle();

		vkDestroyDescriptorPool(_device.GetDevice(), poolIt->Pool, _device.GetAllocationCallbacks());
		poolIt = _pools.erase(poolIt);

		CocoTrace("Destroyed VulkanDescriptorPool for layout ID {}. Pool count is now {}", _layoutID, _pools.size())
	}

	void VulkanDescriptorSetPool::DestroyDescriptorPools()
	{
		_device.WaitForIdle();

		FreeSets();

		auto it = _pools.begin();
		while (it != _pools.end())
			DestroyDescriptorPool(it);

		CocoAssert(_pools.size() == 0, "Pools were not fully released")
	}
}