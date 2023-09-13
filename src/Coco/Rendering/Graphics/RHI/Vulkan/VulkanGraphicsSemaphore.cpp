#include "Renderpch.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsSemaphore.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsSemaphore::VulkanGraphicsSemaphore() :
		_semaphore(nullptr)
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		AssertVkSuccess(vkCreateSemaphore(_device->GetDevice(), &createInfo, _device->GetAllocationCallbacks(), &_semaphore));
	}

	VulkanGraphicsSemaphore::~VulkanGraphicsSemaphore()
	{
		_device->WaitForIdle();

		if (_semaphore)
		{
			vkDestroySemaphore(_device->GetDevice(), _semaphore, _device->GetAllocationCallbacks());
			_semaphore = nullptr;
		}
	}
}