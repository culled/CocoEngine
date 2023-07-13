#include "VulkanRenderCache.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include "../../RenderView.h"
#include "GraphicsDeviceVulkan.h"
#include "VulkanDescriptorSet.h"

namespace Coco::Rendering::Vulkan
{
	VulkanRenderCache::VulkanRenderCache(GraphicsDeviceVulkan* device) :
		_device(device), 
		_pipelineCache(CreateManagedRef<ResourceCache<VulkanPipeline>>(RenderingService::DefaultGraphicsResourceTickLifetime)),
		_renderPassCache(CreateManagedRef<ResourceCache<VulkanRenderPass>>(RenderingService::DefaultGraphicsResourceTickLifetime)),
		_shaderCache(CreateManagedRef<ResourceCache<VulkanShader>>(RenderingService::DefaultGraphicsResourceTickLifetime))
	{}

	VulkanRenderCache::~VulkanRenderCache()
	{
		Invalidate();
	}

	void VulkanRenderCache::Invalidate() noexcept
	{
		_renderPassCache->Invalidate();
		_pipelineCache->Invalidate();
		_shaderCache->Invalidate();
	}

	void VulkanRenderCache::PurgeResources() noexcept
	{
		// Purge renderpass cache
		uint64_t renderPassesPurged = _renderPassCache->PurgeStaleResources();

		// Purge pipeline cache
		uint64_t pipelinesPurged = _pipelineCache->PurgeStaleResources();

		// Purge shader cache
		uint64_t shadersPurged = _shaderCache->PurgeStaleResources();

		if (renderPassesPurged > 0 || pipelinesPurged > 0 || shadersPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString(
				"Purged {} cached renderpasses, {} cached pipelines, and {} cached shaders from global cache",
				renderPassesPurged, 
				pipelinesPurged, 
				shadersPurged
			));
	}

	VulkanRenderPass* VulkanRenderCache::GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline)
	{
		const ResourceID& id = renderPipeline->ID;
		VulkanRenderPass* resource;

		if (!_renderPassCache->Has(id))
			resource = _renderPassCache->Create(id, renderPipeline);
		else
			resource = _renderPassCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating Vulkan RenderPass for pipeline {}", renderPipeline->ID.AsString()));

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

	VulkanPipeline* VulkanRenderCache::GetOrCreatePipeline(
		VulkanRenderPass& renderPass,
		const VulkanShader& shader,
		const string& subshaderName,
		uint32_t subpassIndex,
		const VkDescriptorSetLayout& globalDescriptorLayout)
	{
		const ResourceID& id = VulkanPipeline::GetResourceID(renderPass, shader);
		VulkanPipeline* resource;

		if (!_pipelineCache->Has(id))
			resource = _pipelineCache->Create(id, renderPass, shader, subshaderName, subpassIndex);
		else
			resource = _pipelineCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(renderPass, shader))
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating pipeline for subshader \"{}\" and render pass {}", subshaderName, renderPass.ID.AsString()));

			try
			{
				resource->Update(renderPass, shader, globalDescriptorLayout);
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

	VulkanShader* VulkanRenderCache::GetOrCreateVulkanShader(const ShaderRenderData& shaderData)
	{
		const ResourceID& id = shaderData.ID;
		VulkanShader* resource;

		if (!_shaderCache->Has(id))
			resource = _shaderCache->Create(id, shaderData);
		else
			resource = _shaderCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(shaderData))
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating Vulkan shader for shader {}", shaderData.ID.AsString()));

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