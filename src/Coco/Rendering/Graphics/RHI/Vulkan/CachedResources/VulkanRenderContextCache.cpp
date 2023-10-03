#include "Renderpch.h"
#include "VulkanRenderContextCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const int VulkanRenderContextCache::sPurgePriority = 1000;
	const double VulkanRenderContextCache::sPurgePeriod = 5.0;
	const double VulkanRenderContextCache::sPurgeThreshold = 4.0;

	VulkanRenderContextCache::VulkanRenderContextCache(const GraphicsDeviceResourceID& id) :
		CachedVulkanResource(id)
	{}

	VulkanRenderContextCache::~VulkanRenderContextCache()
	{
		_framebuffers.clear();
	}

	VulkanFramebuffer& VulkanRenderContextCache::GetOrCreateFramebuffer(
		const SizeInt& size, 
		VulkanRenderPass& renderPass,
		std::span<const VulkanImage*> attachmentImages)
	{
		GraphicsDeviceResourceID key = VulkanFramebuffer::MakeKey(renderPass, attachmentImages);

		auto it = _framebuffers.find(key);

		if (it == _framebuffers.end())
		{
			it = _framebuffers.try_emplace(key, renderPass, attachmentImages).first;
		}

		VulkanFramebuffer& resource = it->second;

		if (resource.NeedsUpdate(renderPass, size))
			resource.Update(renderPass, size, attachmentImages);

		resource.Use();

		return resource;
	}
	
	VulkanGlobalUniformData& VulkanRenderContextCache::GetOrCreateGlobalUniformData(const GlobalShaderUniformLayout& layout)
	{
		GraphicsDeviceResourceID key = VulkanGlobalUniformData::MakeKey(layout);

		auto it = _globalUniformDatas.find(key);

		if (it == _globalUniformDatas.end())
		{
			it = _globalUniformDatas.try_emplace(key, layout, nullptr).first;
		}

		VulkanGlobalUniformData& resource = it->second;

		resource.Use();

		return resource;
	}


	VulkanShaderUniformData& VulkanRenderContextCache::GetOrCreateShaderUniformData(const VulkanRenderPassShader& shader)
	{
		GraphicsDeviceResourceID key = VulkanShaderUniformData::MakeKey(shader);

		auto it = _shaderUniformDatas.find(key);

		if (it == _shaderUniformDatas.end())
		{
			it = _shaderUniformDatas.try_emplace(key, shader).first;
		}

		VulkanShaderUniformData& resource = it->second;

		if (resource.NeedsUpdate(shader))
			resource.Update(shader);

		resource.Use();

		return resource;
	}

	void VulkanRenderContextCache::ResetForNextFrame()
	{
		for (auto it = _shaderUniformDatas.begin(); it != _shaderUniformDatas.end(); it++)
			it->second.ResetForNextFrame();
	}

	void VulkanRenderContextCache::PurgeStaleResources()
	{
		uint64 framebuffersPurged = 0;

		{
			auto it = _framebuffers.begin();

			while (it != _framebuffers.end())
			{
				if (it->second.IsStale(sPurgeThreshold))
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

		uint64 globalUniformDatasPurged = 0;

		{
			auto it = _globalUniformDatas.begin();

			while (it != _globalUniformDatas.end())
			{
				if (it->second.IsStale(sPurgeThreshold))
				{
					it = _globalUniformDatas.erase(it);
					globalUniformDatasPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		uint64 shaderUniformDatasPurged = 0;

		{
			auto it = _shaderUniformDatas.begin();

			while (it != _shaderUniformDatas.end())
			{
				if (it->second.IsStale(sPurgeThreshold))
				{
					it = _shaderUniformDatas.erase(it);
					shaderUniformDatasPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		if (framebuffersPurged > 0 || globalUniformDatasPurged  > 0 || shaderUniformDatasPurged > 0)
		{
			CocoTrace("Purged {} VulkanFramebuffers, {} VulkanGlobalUniformDatas, and {} VulkanShaderUniformDatas", 
				framebuffersPurged, globalUniformDatasPurged, shaderUniformDatasPurged)
		}
	}
}