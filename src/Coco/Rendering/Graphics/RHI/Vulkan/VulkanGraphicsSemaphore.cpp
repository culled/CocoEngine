#include "Renderpch.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsDevice.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsSemaphore::VulkanGraphicsSemaphore(const GraphicsResourceID& id, VulkanGraphicsDevice& device) :
		GraphicsResource(id),
		_device(device),
		_semaphore(nullptr)
	{
		VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		AssertVkSuccess(vkCreateSemaphore(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_semaphore))
	}

	VulkanGraphicsSemaphore::~VulkanGraphicsSemaphore()
	{
		if(_semaphore)
		{
			_device.WaitForIdle();
			vkDestroySemaphore(_device.GetDevice(), _semaphore, _device.GetAllocationCallbacks());
			_semaphore = nullptr;
		}
	}
}