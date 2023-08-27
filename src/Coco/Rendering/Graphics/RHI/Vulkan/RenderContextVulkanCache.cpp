#include "RenderContextVulkanCache.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Material.h>
#include "GraphicsDeviceVulkan.h"
#include "Resources/Cache/VulkanShader.h"
#include "Resources/Cache/VulkanShaderResource.h"
#include "Resources/Cache/VulkanFramebuffer.h"
#include "Resources/VulkanDescriptorPool.h"
#include "Resources/BufferVulkan.h"
#include <Coco/Rendering/Graphics/RenderView.h>

namespace Coco::Rendering::Vulkan
{
	RenderContextVulkanCache::RenderContextVulkanCache() : 
		BaseGraphicsResource<GraphicsDeviceVulkan>()
	{
		_shaderCache = CreateManagedRef<ResourceCache<VulkanShaderResource>>(RenderingService::DefaultGraphicsResourceTickLifetime);
		_framebufferCache = CreateManagedRef<ResourceCache<VulkanFramebuffer>>(RenderingService::DefaultGraphicsResourceTickLifetime);
	}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_shaderCache.Reset();
		_framebufferCache.Reset();
	}

	VulkanShaderResource* RenderContextVulkanCache::GetOrCreateShaderResource(const VulkanShader& shader)
	{
		const ResourceID& id = shader.ID;
		VulkanShaderResource* resource;

		if (!_shaderCache->Has(id))
			resource = _shaderCache->Create(id, shader);
		else
			resource = _shaderCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(shader))
			resource->Update(shader);

		return resource;
	}

	VulkanFramebuffer* RenderContextVulkanCache::GetOrCreateFramebuffer(Ref<RenderView>& renderView, VulkanRenderPass& renderPass, Ref<RenderPipeline> pipeline)
	{
		const ResourceID& id = pipeline->ID;
		VulkanFramebuffer* resource;

		if (!_framebufferCache->Has(id))
			resource = _framebufferCache->Create(id, pipeline);
		else
			resource = _framebufferCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(renderView))
			resource->Update(renderView, renderPass);

		return resource;
	}

	void RenderContextVulkanCache::FlushUniformBufferChanges()
	{
		for (auto& shader : *_shaderCache)
			shader.FlushBufferChanges();
	}

	void RenderContextVulkanCache::ResetForNewFrame()
	{
		for (auto& shader : *_shaderCache)
			shader.ResetForNewFrame();
	}

	void RenderContextVulkanCache::PurgeResources() noexcept
	{
		// Purge shader cache
		uint64_t shadersPurged = _shaderCache->PurgeStaleResources();

		// Purge framebuffer cache
		uint64_t framebuffersPurged = _framebufferCache->PurgeStaleResources();

		if (shadersPurged > 0 || framebuffersPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString("Purged {} cached shaders and {} cached framebuffers from RenderContext cache", shadersPurged, framebuffersPurged));
	}
}
