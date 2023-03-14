#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanDescriptorSet.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Manages a set pool of descriptor sets
	/// </summary>
	class DescriptorPoolVulkan : public IGraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;

		uint _maxDescriptorSets;
		Map<uint64_t, VkDescriptorSet> _allocatedDescriptorSets;
		List<VulkanShaderDescriptorLayout> _descriptorSetLayouts;

		VkDescriptorPool _pool;

	public:
		DescriptorPoolVulkan(GraphicsDevice* owningDevice, uint maxSets, const List<VulkanShaderDescriptorLayout>& descriptorSetLayouts);
		virtual ~DescriptorPoolVulkan();

		/// <summary>
		/// Gets or allocates a descriptor set linked to the given unique key
		/// </summary>
		/// <param name="key">The key for the set</param>
		/// <returns></returns>
		VkDescriptorSet GetOrAllocateSet(const VulkanShaderDescriptorLayout& layout, uint64_t key);

		/// <summary>
		/// Frees all allocated descriptor sets
		/// </summary>
		void FreeSets();
	};
}