#include "Renderpch.h"
#include "VulkanGraphicsDeviceCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const double VulkanGraphicsDeviceCache::ResourcePurgePeriod = 1.0;
	const double VulkanGraphicsDeviceCache::StaleResourceThreshold = 1.5;
	const int VulkanGraphicsDeviceCache::ResourcePurgeTickPriority = -200000;

	VulkanGraphicsDeviceCache::VulkanGraphicsDeviceCache(VulkanGraphicsDevice& device) :
		_device(device),
		_purgeTickListener(CreateManagedRef<TickListener>(this, &VulkanGraphicsDeviceCache::HandlePurgeTickListener, ResourcePurgeTickPriority)),
		_renderContextPool(CreateUniqueRef<VulkanRenderContextPool>(device)),
		_renderFramePool(CreateUniqueRef<VulkanRenderFramePool>(device)),
		_meshCache(CreateUniqueRef<VulkanMeshCache>(device, false)),
		_renderPasses(),
		_descriptorSetLayouts(),
		_shaders(),
		_pipelines(),
		_framebuffers()
	{
		_purgeTickListener->SetTickPeriod(ResourcePurgePeriod);
		MainLoop::Get()->AddTickListener(_purgeTickListener);
	}

	VulkanGraphicsDeviceCache::~VulkanGraphicsDeviceCache()
	{
		_renderPasses.clear();
		_descriptorSetLayouts.clear();
		_shaders.clear();
		_pipelines.clear();
		_framebuffers.clear();
		_meshCache.reset();
		_renderFramePool.reset();
		_renderContextPool.reset();

		MainLoop::Get()->RemoveTickListener(_purgeTickListener);
	}

	VulkanRenderPass& VulkanGraphicsDeviceCache::GetOrCreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices)
	{
		uint64 key = VulkanRenderPass::MakeKey(pipeline, samples);
		VulkanRenderPass& renderPass = _renderPasses.try_emplace(key, key, _device).first->second;

		if (renderPass.NeedsUpdate(pipeline, samples))
			renderPass.Update(pipeline, samples, resolveAttachmentIndices);

		renderPass.Use();

		return renderPass;
	}

	VulkanDescriptorSetLayout& VulkanGraphicsDeviceCache::GetOrCreateDescriptorSetLayout(const ShaderUniformLayout& layout, bool includeDataUniforms)
	{
		uint64 key = VulkanDescriptorSetLayout::MakeKey(layout, includeDataUniforms);

		VulkanDescriptorSetLayout& setLayout = _descriptorSetLayouts.try_emplace(key, _device, layout, includeDataUniforms).first->second;

		setLayout.Use();

		return setLayout;
	}

	void VulkanGraphicsDeviceCache::UseDescriptorSetLayout(uint64 setID)
	{
		auto it = _descriptorSetLayouts.find(setID);

		if (it == _descriptorSetLayouts.end())
			return;

		it->second.Use();
	}

	VulkanShader& VulkanGraphicsDeviceCache::GetOrCreateShader(const SharedRef<Shader>& shader)
	{
		CocoAssert(shader, "Shader was invalid")

		uint64 key = VulkanShader::MakeKey(*shader);

		VulkanShader& vulkanShader = _shaders.try_emplace(key, key, _device, shader).first->second;

		vulkanShader.Use();

		return vulkanShader;
	}

	VulkanPipeline& VulkanGraphicsDeviceCache::GetOrCreatePipeline(
		const VulkanRenderPass& renderPass, 
		const VulkanShader& shader, 
		uint32 subpassIndex, 
		const VulkanDescriptorSetLayout& globalLayout)
	{
		uint64 key = VulkanPipeline::MakeKey(renderPass, shader, subpassIndex, globalLayout);

		VulkanPipeline& pipeline = _pipelines.try_emplace(key, key, _device).first->second;

		if (pipeline.NeedsUpdate(renderPass, shader))
			pipeline.Update(renderPass, shader, subpassIndex, globalLayout);

		pipeline.Use();

		return pipeline;
	}

	VulkanFramebuffer& VulkanGraphicsDeviceCache::GetOrCreateFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		uint64 key = VulkanFramebuffer::MakeKey(renderPass, attachmentImages);
		VulkanFramebuffer& framebuffer = _framebuffers.try_emplace(key, key, _device).first->second;

		if (framebuffer.NeedsUpdate(attachmentImages))
			framebuffer.Update(renderPass, attachmentImages);

		framebuffer.Use();
		return framebuffer;
	}

	void VulkanGraphicsDeviceCache::PurgeStaleResources()
	{
		uint64 renderPassesPurged = std::erase_if(_renderPasses,
			[](const auto& kvp)
			{
				return kvp.second.IsStale(StaleResourceThreshold);
			});

		if (renderPassesPurged > 0)
		{
			CocoTrace("Purged {} VulkanRenderPasses", renderPassesPurged)
		}

		uint64 setLayoutsPurged = std::erase_if(_descriptorSetLayouts,
			[](const auto& kvp)
			{
				return kvp.second.IsStale(StaleResourceThreshold);
			});

		if (setLayoutsPurged > 0)
		{
			CocoTrace("Purged {} VulkanDescriptorSetLayouts", setLayoutsPurged)
		}

		uint64 shadersPurged = std::erase_if(_shaders,
			[](const auto& kvp)
			{
				return kvp.second.IsStale(StaleResourceThreshold);
			});

		if (shadersPurged > 0)
		{
			CocoTrace("Purged {} VulkanShaders", shadersPurged)
		}

		uint64 pipelinesPurged = std::erase_if(_pipelines,
			[](const auto& kvp)
			{
				return kvp.second.IsStale(StaleResourceThreshold);
			});

		if (pipelinesPurged > 0)
		{
			CocoTrace("Purged {} VulkanPipelines", pipelinesPurged)
		}

		uint64 framebuffersPurged = std::erase_if(_framebuffers,
			[](const auto& kvp)
			{
				return kvp.second.IsStale(StaleResourceThreshold);
			});

		if (framebuffersPurged > 0)
		{
			CocoTrace("Purged {} VulkanFramebuffers", framebuffersPurged)
		}

		uint64 meshesPurged = _meshCache->PurgeUnusedMeshDatas();

		if (meshesPurged > 0)
		{
			CocoTrace("Purged {} cached mesh datas", meshesPurged)
		}
	}

	void VulkanGraphicsDeviceCache::HandlePurgeTickListener(const TickInfo& tickInfo)
	{
		PurgeStaleResources();
	}
}