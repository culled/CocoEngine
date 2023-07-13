#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include "../VulkanDescriptorSet.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Manages a pool of descriptor sets
	class VulkanDescriptorPool final : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>
	{
	private:
		uint _maxDescriptorSets;
		UnorderedMap<uint64_t, VkDescriptorSet> _allocatedDescriptorSets;
		List<VulkanDescriptorLayout> _descriptorSetLayouts;

		VkDescriptorPool _pool;

	public:
		VulkanDescriptorPool(
			const ResourceID& id, 
			const string& name,  
			uint maxSets, 
			const List<VulkanDescriptorLayout>& descriptorSetLayouts);
		~VulkanDescriptorPool() final;

		DefineResourceType(VulkanDescriptorPool)

		/// @brief Gets or allocates a descriptor set linked to the given unique key
		/// @param layout The layout to allocate descriptors for
		/// @param key The key for the set
		/// @return The allocated set
		VkDescriptorSet GetOrAllocateSet(const VulkanDescriptorLayout& layout, uint64_t key);

		/// @brief Frees all allocated descriptor sets
		void FreeSets() noexcept;
	};
}