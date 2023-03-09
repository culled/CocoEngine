#include "GraphicsSemaphoreVulkan.h"

#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering
{
	GraphicsSemaphoreVulkan::GraphicsSemaphoreVulkan(GraphicsDevice* owningDevice) : GraphicsSemaphore(owningDevice),
		_device(static_cast<GraphicsDeviceVulkan*>(owningDevice))
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