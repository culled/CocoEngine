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

		if (resource.NeedsUpdate(pipeline))
			resource.Update(pipeline);

		resource.Use();

		return resource;
	}

	VulkanRenderPassShader& VulkanGraphicsDeviceCache::GetOrCreateShader(const RenderPassShader& shaderInfo)
	{
		GraphicsDeviceResourceID key = VulkanRenderPassShader::MakeKey(shaderInfo);

		auto it = _shaders.find(key);

		if (it == _shaders.end())
		{
			it = _shaders.try_emplace(key, shaderInfo).first;
		}

		VulkanRenderPassShader& resource = it->second;

		if (resource.NeedsUpdate(shaderInfo))
			resource.Update(shaderInfo);

		resource.Use();

		return resource;
	}

	VulkanPipeline& VulkanGraphicsDeviceCache::GetOrCreatePipeline(
		const VulkanRenderPass& renderPass, 
		uint32 subpassIndex, 
		const VulkanRenderPassShader& shader)
	{
		GraphicsDeviceResourceID key = VulkanPipeline::MakeKey(renderPass, shader, subpassIndex);

		auto it = _pipelines.find(key);

		if (it == _pipelines.end())
		{
			it = _pipelines.try_emplace(key, renderPass, shader, subpassIndex).first;
		}

		VulkanPipeline& resource = it->second;

		if (resource.NeedsUpdate(renderPass, shader))
			resource.Update(renderPass, shader, subpassIndex);

		resource.Use();

		return resource;
	}

	VulkanRenderContextCache& VulkanGraphicsDeviceCache::GetOrCreateContextCache(const GraphicsDeviceResourceID& id)
	{
		auto it = _contextCaches.find(id);

		if (it == _contextCaches.end())
		{
			it = _contextCaches.try_emplace(id).first;
		}

		VulkanRenderContextCache& resource = it->second;

		resource.Use();

		return resource;
	}

	void VulkanGraphicsDeviceCache::ResetForNextFrame()
	{
		for (auto it = _contextCaches.begin(); it != _contextCaches.end(); it++)
			it->second.ResetForNextFrame();
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

		uint64 shadersPurged = 0;

		{
			auto it = _shaders.begin();

			while (it != _shaders.end())
			{
				if (it->second.IsStale())
				{
					it = _shaders.erase(it);
					shadersPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		uint64 pipelinesPurged = 0;

		{
			auto it = _pipelines.begin();

			while (it != _pipelines.end())
			{
				if (it->second.IsStale())
				{
					it = _pipelines.erase(it);
					pipelinesPurged++;
				}
				else
				{
					it++;
				}
			}
		}

		uint64 cachesPurged = 0;
		{
			auto it = _contextCaches.begin();

			while (it != _contextCaches.end())
			{
				if (it->second.IsStale())
				{
					it = _contextCaches.erase(it);
					pipelinesPurged++;
				}
				else
				{
					it->second.PurgeStaleResources();
					it++;
				}
			}
		}

		if (renderPassesPurged > 0 || shadersPurged > 0 || pipelinesPurged > 0 || cachesPurged > 0)
		{
			CocoTrace("Purged {} VulkanRenderPasses, {} VulkanRenderPassShaders, {} VulkanPipelines, and {} VulkanRenderContextCaches", 
				renderPassesPurged, shadersPurged, pipelinesPurged, cachesPurged)
		}
	}

	void VulkanGraphicsDeviceCache::TickCallback(const TickInfo& currentTick)
	{
		PurgeStaleResources();
	}
}