#include "GraphicsSemaphoreVulkan.h"

#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	GraphicsSemaphoreVulkan::GraphicsSemaphoreVulkan(GraphicsDevice* device) :
		_device(static_cast<GraphicsDeviceVulkan*>(device))
	{
		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		AssertVkResult(vkCreateSemaphore(_device->GetDevice(), &createInfo, nullptr, &_semaphore));
	}

	GraphicsSemaphoreVulkan::~GraphicsSemaphoreVulkan()
	{
		if (_semaphore != nullptr)
		{
			vkDestroySemaphore(_device->GetDevice(), _semaphore, nullptr);
			_semaphore = nullptr;
		}
	}
}