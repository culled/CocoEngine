#include "GraphicsSemaphoreVulkan.h"

#include "../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	GraphicsSemaphoreVulkan::GraphicsSemaphoreVulkan(ResourceID id, const string& name) :
		GraphicsResource<GraphicsDeviceVulkan, GraphicsSemaphore>(id, name)
	{
		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		AssertVkResult(vkCreateSemaphore(_device->GetDevice(), &createInfo, nullptr, &_semaphore));
	}

	GraphicsSemaphoreVulkan::~GraphicsSemaphoreVulkan()
	{
		_device->WaitForIdle();

		if (_semaphore != nullptr)
		{
			vkDestroySemaphore(_device->GetDevice(), _semaphore, nullptr);
			_semaphore = nullptr;
		}
	}
}