#include "GraphicsFenceVulkan.h"
#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering
{
	GraphicsFenceVulkan::GraphicsFenceVulkan(GraphicsDevice* owningDevice, bool startSignalled) : GraphicsFence(owningDevice),
		_device(static_cast<GraphicsDeviceVulkan*>(owningDevice))
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		
		if (startSignalled)
			createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

		AssertVkResult(vkCreateFence(_device->GetDevice(), &createInfo, nullptr, &_fence));
	}

	GraphicsFenceVulkan::~GraphicsFenceVulkan()
	{
		if (_fence != nullptr)
		{
			vkDestroyFence(_device->GetDevice(), _fence, nullptr);
			_fence = nullptr;
		}
	}

	bool GraphicsFenceVulkan::IsSignalled() const
	{
		return vkGetFenceStatus(_device->GetDevice(), _fence) == VK_SUCCESS;
	}

	void GraphicsFenceVulkan::Reset()
	{
		AssertVkResult(vkResetFences(_device->GetDevice(), 1, &_fence));
	}

	void GraphicsFenceVulkan::Wait(unsigned long long timeoutNs)
	{
		vkWaitForFences(_device->GetDevice(), 1, &_fence, VK_TRUE, timeoutNs);
	}
}