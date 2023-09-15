#include "Renderpch.h"
#include "VulkanGraphicsDeviceCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const int VulkanGraphicsDeviceCache::sPurgePriority = 1000;
	const double VulkanGraphicsDeviceCache::sPurgePeriod = 5.0;
	const double VulkanGraphicsDeviceCache::sPurgeThreshold = 4.0;

	VulkanGraphicsDeviceCache::VulkanGraphicsDeviceCache() :
		_tickListener(this, &VulkanGraphicsDeviceCache::TickCallback, sPurgePriority)
	{
		_tickListener.SetTickPeriod(sPurgePeriod);

		MainLoop::Get()->AddListener(_tickListener);
	}

	VulkanGraphicsDeviceCache::~VulkanGraphicsDeviceCache()
	{
		MainLoop::Get()->RemoveListener(_tickListener);

		_renderPasses.clear();
	}

	VulkanRenderPass& VulkanGraphicsDeviceCache::GetOrCreateRenderPass(CompiledRenderPipeline& pipeline)
	{
		GraphicsDeviceResourceID key = VulkanRenderPass::MakeKey(pipeline);

		auto it = _renderPasses.find(key);

		if (it == _renderPasses.end())
		{
			it = _renderPasses.try_emplace(key, pipeline).first;
		}

		VulkanRenderPass& resource = it->second;
		resource.Use();

		return resource;
	}

	void VulkanGraphicsDeviceCache::PurgeStaleResources()
	{
		uint64 renderPassesPurged = 0;

		{
			auto it = _renderPasses.begin();

			while (it != _renderPasses.end())
			{
				if (it->second.IsStale())
				{
					it = _renderPasses.erase(it);
					renderPassesPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		if (renderPassesPurged > 0)
		{
			CocoTrace("Purged {} VulkanRenderPasses", renderPassesPurged)
		}
	}

	void VulkanGraphicsDeviceCache::TickCallback(const TickInfo& currentTick)
	{
		PurgeStaleResources();
	}
}