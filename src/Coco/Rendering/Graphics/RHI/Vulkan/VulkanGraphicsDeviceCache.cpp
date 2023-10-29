#include "Renderpch.h"
#include "VulkanGraphicsDeviceCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const double VulkanGraphicsDeviceCache::sPurgePeriod = 5.0;
	const double VulkanGraphicsDeviceCache::sPurgeThreshold = 4.0;

	VulkanGraphicsDeviceCache::VulkanGraphicsDeviceCache() :
		_lastPurgeTime(0.0)
	{}

	VulkanGraphicsDeviceCache::~VulkanGraphicsDeviceCache()
	{
		_renderPasses.clear();
	}

	VulkanRenderPass& VulkanGraphicsDeviceCache::GetOrCreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices)
	{
		GraphicsDeviceResourceID key = VulkanRenderPass::MakeKey(pipeline, samples);

		auto it = _renderPasses.find(key);

		if (it == _renderPasses.end())
		{
			it = _renderPasses.try_emplace(key, pipeline, samples).first;
		}

		VulkanRenderPass& resource = it->second;

		if (resource.NeedsUpdate(pipeline, samples))
			resource.Update(pipeline, samples, resolveAttachmentIndices);

		resource.Use();

		return resource;
	}

	VulkanShaderVariant& VulkanGraphicsDeviceCache::GetOrCreateShader(const ShaderVariantData& variantData)
	{
		GraphicsDeviceResourceID key = VulkanShaderVariant::MakeKey(variantData);

		auto it = _shaders.find(key);

		if (it == _shaders.end())
		{
			it = _shaders.try_emplace(key, variantData).first;
		}

		VulkanShaderVariant& resource = it->second;

		if (resource.NeedsUpdate(variantData))
			resource.Update(variantData);

		resource.Use();

		return resource;
	}

	VulkanPipeline& VulkanGraphicsDeviceCache::GetOrCreatePipeline(
		const VulkanRenderPass& renderPass, 
		uint32 subpassIndex, 
		const VulkanShaderVariant& shader,
		const VulkanDescriptorSetLayout* globalDescriptorSetLayout)
	{
		GraphicsDeviceResourceID key = VulkanPipeline::MakeKey(renderPass, shader, subpassIndex, globalDescriptorSetLayout);

		auto it = _pipelines.find(key);

		if (it == _pipelines.end())
		{
			it = _pipelines.try_emplace(key, renderPass, shader, subpassIndex, globalDescriptorSetLayout).first;
		}

		VulkanPipeline& resource = it->second;

		if (resource.NeedsUpdate(renderPass, shader))
			resource.Update(renderPass, shader, subpassIndex, globalDescriptorSetLayout);

		resource.Use();

		return resource;
	}

	VulkanRenderContextCache& VulkanGraphicsDeviceCache::GetOrCreateContextCache(const GraphicsDeviceResourceID& id)
	{
		auto it = _contextCaches.find(id);

		if (it == _contextCaches.end())
		{
			it = _contextCaches.try_emplace(id, id).first;
		}

		VulkanRenderContextCache& resource = it->second;

		resource.Use();

		return resource;
	}

	void VulkanGraphicsDeviceCache::ResetForNextFrame()
	{
		if (MainLoop::cGet()->GetCurrentTick().UnscaledTime - _lastPurgeTime > sPurgePeriod)
			PurgeStaleResources();

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
				if (it->second.IsStale(sPurgeThreshold))
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
				if (it->second.IsStale(sPurgeThreshold))
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
				if (it->second.IsStale(sPurgeThreshold))
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
				if (it->second.IsStale(sPurgeThreshold))
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

		_lastPurgeTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}
}