#pragma once

#include "../../../Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include "../../GraphicsDeviceResource.h"
#include "VulkanCommandBuffer.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A pool that can allocate VulkanCommandBuffers
	class VulkanCommandBufferPool
	{
	private:
		VulkanGraphicsDevice* _device;
		VkQueue _queue;
		uint32 _queueFamilyIndex;
		VkCommandPool _pool;
		std::vector<VkCommandBuffer> _allocatedBuffers;

	public:
		VulkanCommandBufferPool(VulkanGraphicsDevice* device, VkQueue queue, uint32 queueFamilyIndex);
		~VulkanCommandBufferPool();

		/// @brief Allocates a command buffer
		/// @param topLevel If true, the buffer is top level and cannot be used within another buffer
		/// @return The command buffer
		UniqueRef<VulkanCommandBuffer> Allocate(bool topLevel);

		/// @brief Frees a command buffer
		/// @param buffer The buffer
		void Free(const VulkanCommandBuffer& buffer);
	};
}