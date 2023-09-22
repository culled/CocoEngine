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

	VulkanFramebuffer& VulkanRenderContextCache::GetOrCreateFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		GraphicsDeviceResourceID key = VulkanFramebuffer::MakeKey(size, renderPass, attachmentImages);

		auto it = _framebuffers.find(key);

		if (it == _framebuffers.end())
		{
			it = _framebuffers.try_emplace(key, size, renderPass, attachmentImages).first;
		}

		VulkanFramebuffer& resource = it->second;

		if (resource.NeedsUpdate(renderPass))
			resource.Update(size, renderPass, attachmentImages);

		resource.Use();

		return resource;
	}

	VulkanUniformData& VulkanRenderContextCache::GetOrCreateUniformData(const VulkanRenderPassShader& shader)
	{
		GraphicsDeviceResourceID key = VulkanUniformData::MakeKey(shader);

		auto it = _uniformDatas.find(key);

		if (it == _uniformDatas.end())
		{
			it = _uniformDatas.try_emplace(key, shader).first;
		}

		VulkanUniformData& resource = it->second;

		if (resource.NeedsUpdate(shader))
			resource.Update(shader);

		resource.Use();

		return resource;
	}

	void VulkanRenderContextCache::ResetForNextFrame()
	{
		for (auto it = _uniformDatas.begin(); it != _uniformDatas.end(); it++)
			it->second.ResetForNextFrame();
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

		uint64 uniformDatasPurged = 0;

		{
			auto it = _uniformDatas.begin();

			while (it != _uniformDatas.end())
			{
				if (it->second.IsStale())
				{
					it = _uniformDatas.erase(it);
					uniformDatasPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		if (framebuffersPurged > 0 || uniformDatasPurged > 0)
		{
			CocoTrace("Purged {} VulkanFramebuffers and {} VulkanUniformDatas", framebuffersPurged, uniformDatasPurged)
		}
	}

	void VulkanRenderContextCache::Use()
	{
		_lastUsedTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanRenderContextCache::IsStale() const
	{
		double currentTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanRenderContextCache::sPurgeThreshold;
	}

	void VulkanRenderContextCache::TickCallback(const TickInfo& currentTick)
	{
		PurgeStaleResources();
	}
}