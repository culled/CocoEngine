#pragma once

#include "Pools/VulkanRenderContextPool.h"
#include <Coco/Core/MainLoop/TickListener.h>
#include "CachedResources/VulkanRenderPass.h"
#include "CachedResources/VulkanDescriptorSetLayout.h"
#include "CachedResources/VulkanShader.h"
#include "CachedResources/VulkanPipeline.h"
#include "CachedResources/VulkanFramebuffer.h"
#include "Pools/VulkanRenderFramePool.h"
#include "VulkanMeshCache.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	class VulkanGraphicsDeviceCache
	{
	public:
		static const double ResourcePurgePeriod;
		static const double StaleResourceThreshold;
		static const int ResourcePurgeTickPriority;

	public:
		VulkanGraphicsDeviceCache(VulkanGraphicsDevice& device);
		~VulkanGraphicsDeviceCache();

		VulkanRenderContextPool& GetRenderContextPool() { return *_renderContextPool; }
		VulkanRenderFramePool& GetRenderFramePool() { return *_renderFramePool; }
		VulkanMeshCache& GetMeshCache() { return *_meshCache; }

		VulkanRenderPass& GetOrCreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices);
		VulkanDescriptorSetLayout& GetOrCreateDescriptorSetLayout(const ShaderUniformLayout& layout, bool includeDataUniforms);
		void UseDescriptorSetLayout(uint64 setID);

		VulkanShader& GetOrCreateShader(const SharedRef<Shader>& shader);

		VulkanPipeline& GetOrCreatePipeline(const VulkanRenderPass& renderPass,
			const VulkanShader& shader,
			uint32 subpassIndex,
			const VulkanDescriptorSetLayout& globalLayout);

		VulkanFramebuffer& GetOrCreateFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

	private:
		VulkanGraphicsDevice& _device;
		ManagedRef<TickListener> _purgeTickListener;

		UniqueRef<VulkanRenderContextPool> _renderContextPool;
		UniqueRef<VulkanRenderFramePool> _renderFramePool;
		UniqueRef<VulkanMeshCache> _meshCache;
		std::unordered_map<uint64, VulkanRenderPass> _renderPasses;
		std::unordered_map<uint64, VulkanDescriptorSetLayout> _descriptorSetLayouts;
		std::unordered_map<uint64, VulkanShader> _shaders;
		std::unordered_map<uint64, VulkanPipeline> _pipelines;
		std::unordered_map<uint64, VulkanFramebuffer> _framebuffers;

	private:
		void PurgeStaleResources();
		void HandlePurgeTickListener(const TickInfo& tickInfo);
	};
}