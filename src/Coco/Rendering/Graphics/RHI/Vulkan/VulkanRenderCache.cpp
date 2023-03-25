#include "VulkanRenderCache.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include "../../RenderView.h"
#include "GraphicsDeviceVulkan.h"
#include "Cache/VulkanSubshader.h"
#include "Cache/VulkanRenderPass.h"
#include "Cache/VulkanPipeline.h"
#include "Resources/VulkanDescriptorSet.h"

namespace Coco::Rendering::Vulkan
{
	CachedShader::CachedShader(const ShaderRenderData& shaderData) : CachedResource(shaderData.ID, shaderData.Version) 
	{}

	VulkanRenderCache::VulkanRenderCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{}

	VulkanRenderCache::~VulkanRenderCache()
	{
		Invalidate();
	}

	void VulkanRenderCache::Invalidate() noexcept
	{
		_renderPassCache.clear();
		_pipelineCache.clear();
		_shaderCache.clear();
	}

	void VulkanRenderCache::PurgeResources() noexcept
	{
		// Purge renderpass cache
		uint64_t renderPassesPurged = 0;
		auto renderPassIt = _renderPassCache.begin();
		while (renderPassIt != _renderPassCache.end())
		{
			const Ref<VulkanRenderPass>& resource = (*renderPassIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				renderPassIt = _renderPassCache.erase(renderPassIt);
				renderPassesPurged++;
			}
			else
				renderPassIt++;
		}

		// Purge pipeline cache
		uint64_t pipelinesPurged = 0;
		auto pipelineIt = _pipelineCache.begin();
		while (pipelineIt != _pipelineCache.end())
		{
			const Ref<VulkanPipeline>& resource = (*pipelineIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				pipelineIt = _pipelineCache.erase(pipelineIt);
				pipelinesPurged++;
			}
			else
				pipelineIt++;
		}

		// Purge shader cache
		uint64_t shadersPurged = 0;
		auto shaderIt = _shaderCache.begin();
		while (shaderIt != _shaderCache.end())
		{
			const Ref<CachedShader>& resource = (*shaderIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				shaderIt = _shaderCache.erase(shaderIt);
				shadersPurged++;
			}
			else
				shaderIt++;
		}

		if (renderPassesPurged > 0 || pipelinesPurged > 0 || shadersPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString(
				"Purged {} cached renderpasses, {} cached pipelines, and {} cached shaders from global cache",
				renderPassesPurged, 
				pipelinesPurged, 
				shadersPurged
			));
	}

	Ref<VulkanRenderPass> VulkanRenderCache::GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline)
	{
		const ResourceID id = renderPipeline->ID;

		if (!_renderPassCache.contains(id) || _renderPassCache.at(id)->IsInvalid())
			_renderPassCache[id] = CreateRef<VulkanRenderPass>(_device, renderPipeline);

		Ref<VulkanRenderPass>& resource = _renderPassCache[id];
		Assert(resource->IsInvalid() == false);

		resource->UpdateTickUsed();

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating Vulkan RenderPass for pipeline {}", renderPipeline->ID));

			try
			{
				resource->Update();
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Failed to create Vulkan RenderPass for pipeline: {}", ex.what()));
			}
		}

		return resource;
	}

	Ref<VulkanPipeline> VulkanRenderCache::GetOrCreatePipeline(
		const Ref<VulkanRenderPass>& renderPass,
		const Ref<VulkanSubshader>& subshader,
		uint32_t subpassIndex,
		const VkDescriptorSetLayout& globalDescriptorLayout)
	{
		const uint64_t key = Math::CombineHashes(0, renderPass->ID, subshader->ShaderID);

		if (!_pipelineCache.contains(key) || _pipelineCache.at(key)->IsInvalid())
			_pipelineCache[key] = CreateRef<VulkanPipeline>(_device, renderPass, subshader, subpassIndex);

		Ref<VulkanPipeline>& resource = _pipelineCache[key];
		Assert(resource->IsInvalid() == false);

		resource->UpdateTickUsed();

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating pipeline for subshader \"{}\" and render pass {}", subshader->SubshaderName, renderPass->ID));

			try
			{
				resource->Update(globalDescriptorLayout);
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString(
					"Failed to create pipeline: {}",
					ex.what()
				));
			}
		}

		return resource;
	}

	Ref<VulkanSubshader> VulkanRenderCache::GetOrCreateVulkanSubshader(const string& subshaderName, const ShaderRenderData& shaderData)
	{
		if (!_shaderCache.contains(shaderData.ID))
			_shaderCache.emplace(shaderData.ID, CreateRef<CachedShader>(shaderData));

		Ref<CachedShader>& shaderResource = _shaderCache.at(shaderData.ID);
		if (!shaderResource->Subshaders.contains(subshaderName))
			shaderResource->Subshaders.emplace(subshaderName, CreateRef<VulkanSubshader>(_device, shaderData, subshaderName));

		Ref<VulkanSubshader>& resource = shaderResource->Subshaders.at(subshaderName);

		shaderResource->UpdateTickUsed();

		if (resource->NeedsUpdate(shaderData))
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating Vulkan shader for shader {}, pass \"{}\"", shaderData.ID, subshaderName));

			try
			{
				resource->Update(shaderData);
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Failed to create a Vulkan subshader: {}",
					ex.what()
				));
			}
		}

		return resource;
	}
}