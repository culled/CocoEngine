#include "Renderpch.h"
#include "VulkanRenderFramePool.h"
#include "../VulkanGraphicsDevice.h"

#include <Coco/Core/Engine.h>
#include "../../../../RenderService.h"

namespace Coco::Rendering::Vulkan
{
	VulkanRenderFramePool::VulkanRenderFramePool(VulkanGraphicsDevice& device) :
		_device(device),
		_frames()
	{}

	VulkanRenderFramePool::~VulkanRenderFramePool()
	{
		_frames.clear();
	}

	SharedRef<VulkanRenderFrame> VulkanRenderFramePool::Get()
	{
		for (const SharedRef<VulkanRenderFrame>& frame : _frames)
		{
			if (frame.use_count() > 1 && !frame->IsRenderComplete())
				continue;

			frame->Reset();
			return frame;
		}

		return _frames.emplace_back(CreateSharedRef<VulkanRenderFrame>(_device));
	}
}