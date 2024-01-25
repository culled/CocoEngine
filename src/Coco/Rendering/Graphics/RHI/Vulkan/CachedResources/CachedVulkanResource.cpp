#include "Renderpch.h"
#include "CachedVulkanResource.h"

#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::Rendering::Vulkan
{
	void CachedVulkanResource::Use()
	{
		_lastUseTime = MainLoop::Get()->GetCurrentTick().Time;
	}

	bool CachedVulkanResource::IsStale(double staleThreshold) const
	{
		return MainLoop::Get()->GetCurrentTick().Time - _lastUseTime > staleThreshold;
	}

	CachedVulkanResource::CachedVulkanResource(const uint64& id) :
		ID(id),
		_lastUseTime(MainLoop::Get()->GetCurrentTick().Time)
	{}
}