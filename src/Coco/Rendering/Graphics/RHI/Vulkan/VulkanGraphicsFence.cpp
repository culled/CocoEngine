#include "Renderpch.h"
#include "VulkanGraphicsFence.h"
#include "VulkanGraphicsDevice.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsFence::VulkanGraphicsFence(const GraphicsDeviceResourceID& id, bool startSignaled) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_fence(nullptr)
	{
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (startSignaled)
			createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

		AssertVkSuccess(vkCreateFence(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_fence));
	}

	VulkanGraphicsFence::~VulkanGraphicsFence()
	{
		_device.WaitForIdle();

		if (_fence)
		{
			vkDestroyFence(_device.GetDevice(), _fence, _device.GetAllocationCallbacks());
			_fence = nullptr;
		}
	}

	bool VulkanGraphicsFence::IsSignaled() const
	{
		return vkGetFenceStatus(_device.GetDevice(), _fence) == VK_SUCCESS;
	}

	void VulkanGraphicsFence::Reset()
	{
		AssertVkSuccess(vkResetFences(_device.GetDevice(), 1, &_fence));
	}

	void VulkanGraphicsFence::Wait(uint64 timeoutNs)
	{
		vkWaitForFences(_device.GetDevice(), 1, &_fence, VK_TRUE, timeoutNs);
	}
}