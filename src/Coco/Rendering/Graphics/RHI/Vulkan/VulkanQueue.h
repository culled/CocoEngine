#pragma once
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include "Pools/VulkanCommandBufferPool.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	enum VulkanQueueType
	{
		Graphics,
		Compute,
		Transfer,
		Present
	};

	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanGraphicsDevice& device, uint32 familyIndex, VkQueue queue, VulkanQueueType type);
		~VulkanQueue();

		uint32 GetFamilyIndex() const { return _familyIndex; }
		VkQueue GetQueue() const { return _queue; }
		VulkanQueueType GetType() const { return _type; }
		Ref<VulkanCommandBufferPool> GetCommandPool() { return _commandBufferPool; }

		void WaitForIdle();

	private:
		VulkanGraphicsDevice& _device;

		uint32 _familyIndex;
		VkQueue _queue;
		VulkanQueueType _type;

		ManagedRef<VulkanCommandBufferPool> _commandBufferPool;
	};
}