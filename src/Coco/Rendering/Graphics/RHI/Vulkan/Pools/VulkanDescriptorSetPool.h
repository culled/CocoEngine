#pragma once

#include "../../../../Renderpch.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;
	class VulkanDescriptorSetLayout;

	/// @brief A pool that can allocate descriptor sets
	struct AllocatedVulkanDescriptorPool
	{
		/// @brief The pool
		VkDescriptorPool Pool;

		/// @brief The time that this pool was last allocated from
		double LastAllocateTime;

		AllocatedVulkanDescriptorPool();
	};

	class VulkanDescriptorSetPool
	{
	public:
		VulkanDescriptorSetPool(VulkanGraphicsDevice& device, const VulkanDescriptorSetLayout& layout);
		~VulkanDescriptorSetPool();

		bool TryGetAllocatedSet(uint64 setKey, VkDescriptorSet& outSet) const;
		VkDescriptorSet AllocateDescriptorSet(uint64 setKey);

		void FreeSets();

	private:
		using PoolList = std::vector<AllocatedVulkanDescriptorPool>;

		static const uint32 _maxSets;

		VulkanGraphicsDevice& _device;
		uint64 _layoutID;
		VkDescriptorSetLayout _setLayout;
		VkDescriptorPoolCreateInfo _createInfo;
		std::vector<VkDescriptorPoolSize> _poolSizes;
		PoolList _pools;
		std::unordered_map<uint64, VkDescriptorSet> _allocatedSets;

	private:
		static std::unordered_map<VkDescriptorType, uint32> GetTypeCounts(const VulkanDescriptorSetLayout& layout);
		AllocatedVulkanDescriptorPool& CreateDescriptorPool();
		void DestroyDescriptorPool(PoolList::iterator& poolIt);
		void DestroyDescriptorPools();
	};
}