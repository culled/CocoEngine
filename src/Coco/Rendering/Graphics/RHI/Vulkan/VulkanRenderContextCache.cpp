#include "Renderpch.h"
#include "VulkanRenderContextCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const int VulkanRenderContextCache::sPurgePriority = 1000;
	const double VulkanRenderContextCache::sPurgePeriod = 5.0;
	const double VulkanRenderContextCache::sPurgeThreshold = 4.0;

	VulkanRenderContextCache::VulkanRenderContextCache() :
		_tickListener(this, &VulkanRenderContextCache::TickCallback, sPurgePriority)
	{
		_tickListener.SetTickPeriod(sPurgePeriod);

		MainLoop::Get()->AddListener(_tickListener);
	}

	VulkanRenderContextCache::~VulkanRenderContextCache()
	{
		MainLoop::Get()->RemoveListener(_tickListener);

		_framebuffers.clear();
	}

	VulkanFramebuffer& VulkanRenderContextCache::GetOrCreateFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages)
	{
		GraphicsDeviceResourceID key = VulkanFramebuffer::MakeKey(size, renderPass, attachmentImages);

		auto it = _framebuffers.find(key);

		if (it == _framebuffers.end())
		{
			it = _framebuffers.try_emplace(key, size, renderPass, attachmentImages).first;
		}

		VulkanFramebuffer& resource = it->second;
		resource.Use();

		return resource;
	}

	void VulkanRenderContextCache::PurgeStaleResources()
	{
		uint64 framebuffersPurged = 0;

		{
			auto it = _framebuffers.begin();

			while (it != _framebuffers.end())
			{
				if (it->second.IsStale())
				{
					it = _framebuffers.erase(it);
					framebuffersPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		if (framebuffersPurged > 0)
		{
			CocoTrace("Purged {} VulkanFramebuffers", framebuffersPurged)
		}
	}

	void VulkanRenderContextCache::TickCallback(const TickInfo& currentTick)
	{
		PurgeStaleResources();
	}
}