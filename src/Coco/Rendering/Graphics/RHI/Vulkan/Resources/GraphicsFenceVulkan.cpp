#include "GraphicsFenceVulkan.h"

#include "../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
	GraphicsFenceVulkan::GraphicsFenceVulkan(ResourceID id, const string& name, uint64_t lifetime, bool startSignalled) :
		GraphicsResource<GraphicsDeviceVulkan, GraphicsFence>(id, name, lifetime)
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		
		if (startSignalled)
			createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

		AssertVkResult(vkCreateFence(_device->GetDevice(), &createInfo, nullptr, &_fence));
	}

	GraphicsFenceVulkan::~GraphicsFenceVulkan()
	{
		_device->WaitForIdle();

		if (_fence != nullptr)
		{
			vkDestroyFence(_device->GetDevice(), _fence, nullptr);
			_fence = nullptr;
		}
	}

	bool GraphicsFenceVulkan::IsSignalled() const noexcept
	{
		return vkGetFenceStatus(_device->GetDevice(), _fence) == VK_SUCCESS;
	}

	void GraphicsFenceVulkan::Reset()
	{
		AssertVkResult(vkResetFences(_device->GetDevice(), 1, &_fence));
	}

	void GraphicsFenceVulkan::Wait(uint64_t timeoutNs)
	{
		vkWaitForFences(_device->GetDevice(), 1, &_fence, VK_TRUE, timeoutNs);
	}
}