#include "Renderpch.h"
#include "VulkanRenderContext.h"

namespace Coco::Rendering::Vulkan
{
	VulkanRenderContext::VulkanRenderContext() :
		_backbufferIndex(-1),
		_imageAvailableSemaphore(CreateUniqueRef<VulkanGraphicsSemaphore>()),
		_renderCompletedSemaphore(CreateUniqueRef<VulkanGraphicsSemaphore>()),
		_renderCompletedFence(CreateUniqueRef<VulkanGraphicsFence>(true))
	{
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		_imageAvailableSemaphore.reset();
		_renderCompletedSemaphore.reset();
		_renderCompletedFence.reset();
	}

	void VulkanRenderContext::WaitForRenderingToComplete()
	{
		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
	}

	void VulkanRenderContext::SetBackbufferIndex(uint32 index)
	{
		_backbufferIndex = static_cast<int>(index);
	}

	void VulkanRenderContext::ResetImpl()
	{
		_backbufferIndex = -1;
	}
}