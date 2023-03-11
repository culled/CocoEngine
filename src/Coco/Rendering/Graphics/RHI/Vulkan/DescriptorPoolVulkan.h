#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	// TODO: probably need to overhaul this to make more sense. Just need to figure out the proper way this will work in the backend

	/// <summary>
	/// Manages a set pool of descriptor sets
	/// </summary>
	class DescriptorPoolVulkan : public IGraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;

		uint _maxDescriptorSets;
		Map<uint64_t, VkDescriptorSet> _allocatedDescriptorSets;

		VkDescriptorPool _pool;
		VkDescriptorSetLayout _descriptorSetLayout;

	public:
		DescriptorPoolVulkan(GraphicsDevice* owningDevice, uint maxSets, VkDescriptorSetLayout descriptorSetLayout);
		virtual ~DescriptorPoolVulkan();

		/// <summary>
		/// Gets or allocates a descriptor set linked to the given unique key
		/// </summary>
		/// <param name="key">The key for the set</param>
		/// <returns></returns>
		VkDescriptorSet GetOrAllocateSet(uint64_t key);
		//void FreeSet(uint64_t key);
	};
}