#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	class Shader;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanShader;

	/// <summary>
	/// Information for a RenderPass Subpass
	/// </summary>
	struct SubpassInfo
	{
		List<AttachmentDescription> ColorAttachments;
		List<VkAttachmentReference> ColorAttachmentReferences;
		Optional<AttachmentDescription> DepthStencilAttachment;
		Optional<VkAttachmentReference> DepthStencilAttachmentReference;
		List<uint32_t> PreserveAttachments;

		VkSubpassDescription SubpassDescription = {};
	};

	/// <summary>
	/// A cached Vulkan renderpass
	/// </summary>
	struct CachedVulkanRenderPass final : public CachedResource
	{
		GraphicsDeviceVulkan* Device;
		WeakRef<RenderPipeline> PipelineRef;
		List<SubpassInfo> Subpasses;
		VkRenderPass RenderPass = nullptr;

		CachedVulkanRenderPass(GraphicsDeviceVulkan* device, Ref<RenderPipeline> pipeline);
		~CachedVulkanRenderPass() final;

		bool IsInvalid() const noexcept final { return PipelineRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// <summary>
		/// Deletes the internal Vulkan renderpass
		/// </summary>
		void DeleteRenderPass() noexcept;
	};

	/// <summary>
	/// A cached Vulkan pipeline
	/// </summary>
	struct CachedVulkanPipeline final : public CachedResource
	{
		GraphicsDeviceVulkan* Device;
		WeakRef<RenderPipeline> PipelineRef;
		WeakRef<Shader> ShaderRef;
		VkPipelineLayout Layout = nullptr;
		VkPipeline Pipeline = nullptr;

		CachedVulkanPipeline(GraphicsDeviceVulkan* device, Ref<RenderPipeline> pipeline, Ref<Shader> shader);
		~CachedVulkanPipeline() final;

		bool IsInvalid() const noexcept final { return PipelineRef.expired() || ShaderRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// <summary>
		/// Deletes the internal Vulkan pipeline and its layout
		/// </summary>
		void DeletePipeline() noexcept;
	};

	/// <summary>
	/// A cache for global Vulkan objects (render passes, pipelines, and shaders)
	/// </summary>
	class VulkanRenderCache
	{
	private:
		static constexpr uint64_t s_staleTickCount = 500;

		GraphicsDeviceVulkan* _device;

		Map<ResourceID, Ref<CachedVulkanRenderPass>> _renderPassCache;
		Map<uint64_t, Ref<CachedVulkanPipeline>> _pipelineCache;
		Map<ResourceID, Ref<VulkanShader>> _shaderCache;

	public:
		VulkanRenderCache(GraphicsDeviceVulkan* device);
		~VulkanRenderCache();

		/// <summary>
		/// Clears all cached objects
		/// </summary>
		void Invalidate() noexcept;

		/// <summary>
		/// Purges unused and invalid resources
		/// </summary>
		void PurgeResources() noexcept;

		/// <summary>
		/// Gets or creates a VulkanRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		/// <returns>The VulkanRenderPass for the pipeline</returns>
		Ref<CachedVulkanRenderPass> GetOrCreateRenderPass(Ref<RenderPipeline> renderPipeline);

		/// <summary>
		/// Gets or creates a VulkanPipeline for a render pass and shader combination
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="subpassName">The name of the subpass</param>
		/// <param name="subpassIndex">The index of the subpass in the pipeline</param>
		/// <param name="shader">The shader</param>
		/// <returns>The VulkanPipeline for the shader and render pass</returns>
		Ref<CachedVulkanPipeline> GetOrCreatePipeline(
			const CachedVulkanRenderPass& renderPass,
			const string& subpassName,
			uint32_t subpassIndex, 
			Ref<Shader> shader,
			VkDescriptorSetLayout globalDescriptorLayout);

		/// <summary>
		/// Gets or creates a VulkanShader for a shader
		/// </summary>
		/// <param name="shader">The shader</param>
		/// <returns>The Vulkan-ready shader</returns>
		Ref<VulkanShader> GetOrCreateVulkanShader(Ref<Shader> shader);

	private:
		/// <summary>
		/// Creates a VulkanRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		void CreateRenderPass(CachedVulkanRenderPass& cachedRenderPass);

		/// <summary>
		/// Creates a VulkanPipeline for a render pass and shader combination
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="subpassName">The name of the subpass</param>
		/// <param name="subpassIndex">The index of the subpass in the pipeline</param>
		/// <param name="shader">The shader</param>
		void CreatePipeline(
			const CachedVulkanRenderPass& renderPass,
			const string& subpassName, 
			uint32_t subpassIndex, 
			const Ref<Shader>& shader,
			VkDescriptorSetLayout globalDescriptorLayout,
			CachedVulkanPipeline& cachedPipeline);
	};
}