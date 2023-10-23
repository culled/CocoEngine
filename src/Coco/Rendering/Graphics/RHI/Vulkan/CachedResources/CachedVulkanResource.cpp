#include "Renderpch.h"
#include "CachedVulkanResource.h"
#include "../VulkanGraphicsDevice.h"

#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::Rendering::Vulkan
{
	CachedVulkanResource::CachedVulkanResource(const GraphicsDeviceResourceID& id) : 
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_lastUseTime(0.0)
	{}

	void CachedVulkanResource::Use()
	{
		_lastUseTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool CachedVulkanResource::IsStale(double staleThreshold) const
	{
		return MainLoop::cGet()->GetCurrentTick().UnscaledTime - _lastUseTime > staleThreshold;
	}
}