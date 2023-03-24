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

	/// @brief Information for a RenderPass Subpass
	struct SubpassInfo
	{
		/// @brief The color attachment descriptions
		List<AttachmentDescription> ColorAttachments;

		/// @brief The color attachment references
		List<VkAttachmentReference> ColorAttachmentReferences;

		/// @brief The depth-stencil attachment description
		Optional<AttachmentDescription> DepthStencilAttachment;

		/// @brief The depth-stencil attachment reference
		Optional<VkAttachmentReference> DepthStencilAttachmentReference;

		/// @brief Indices of attachments to preserve
		List<uint32_t> PreserveAttachments;

		/// @brief The subpass description
		VkSubpassDescription SubpassDescription = {};
	};

	/// @brief A cached Vulkan renderpass
	struct CachedVulkanRenderPass final : public CachedResource
	{
		/// @brief A pointer to the graphics device
		GraphicsDeviceVulkan* const Device;

		/// @brief The pipeline that this render pass was created from
		WeakRef<RenderPipeline> PipelineRef;

		/// @brief The subpass infos of this render pass
		List<SubpassInfo> Subpasses;

		/// @brief The Vulkan render pass
		VkRenderPass RenderPass = nullptr;

		CachedVulkanRenderPass(GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline);
		~CachedVulkanRenderPass() final;

		bool IsInvalid() const noexcept final { return PipelineRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Destroys the internal Vulkan renderpass
		void DestroyRenderPass() noexcept;
	};

	/// @brief A cached Vulkan pipeline
	struct CachedVulkanPipeline final : public CachedResource
	{
		/// @brief A pointer to the graphics device
		GraphicsDeviceVulkan* const Device;

		/// @brief The pipeline that this pipeline was created from
		WeakRef<RenderPipeline> PipelineRef;

		/// @brief The shader that this pipeline was created from
		WeakRef<Shader> ShaderRef;

		/// @brief The Vulkan pipeline layout
		VkPipelineLayout Layout = nullptr;

		/// @brief The Vulkan pipeline
		VkPipeline Pipeline = nullptr;

		CachedVulkanPipeline(GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline, const Ref<Shader>& shader);
		~CachedVulkanPipeline() final;

		bool IsInvalid() const noexcept final { return PipelineRef.expired() || ShaderRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Destroys the internal Vulkan pipeline and its layout
		void DestroyPipeline() noexcept;
	};

	/// @brief A cache for global Vulkan objects (render passes, pipelines, and shaders)
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

		/// @brief Clears all cached objects
		void Invalidate() noexcept;

		/// @brief Purges unused and invalid resources
		void PurgeResources() noexcept;

		/// @brief Gets or creates a render pass resource for a RenderPipeline
		/// @param renderPipeline The pipeline
		/// @return The render pass resource
		Ref<CachedVulkanRenderPass> GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline);

		/// @brief Gets or creates a pipeline for a render pass and shader combination
		/// @param renderPass The render pass
		/// @param subpassName The name of the subpass
		/// @param subpassIndex The index of the subpass in the pipeline
		/// @param shader The shader
		/// @param globalDescriptorLayout The layout for the global descriptors
		/// @return The pipeline resource
		Ref<CachedVulkanPipeline> GetOrCreatePipeline(
			const CachedVulkanRenderPass& renderPass,
			const string& subpassName,
			uint32_t subpassIndex, 
			const Ref<Shader>& shader,
			const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets or creates a Vulkan shader for a shader
		/// @param shader The shader
		/// @return The Vulkan shader resource
		Ref<VulkanShader> GetOrCreateVulkanShader(const Ref<Shader>& shader);

	private:
		/// @brief Creates a render pass
		/// @param cachedRenderPass The cached resource that will hold the created render pass
		void CreateRenderPass(CachedVulkanRenderPass& cachedRenderPass);

		/// @brief Creates a pipeline
		/// @param renderPass The render pass
		/// @param subpassName The name of the subpass
		/// @param subpassIndex The index of the subpass in the pipeline
		/// @param shader The shader
		/// @param globalDescriptorLayout The layout for the global descriptors
		/// @param The cached resource that will hold the pipeline
		void CreatePipeline(
			const CachedVulkanRenderPass& renderPass,
			const string& subpassName, 
			uint32_t subpassIndex, 
			const Ref<Shader>& shader,
			const VkDescriptorSetLayout& globalDescriptorLayout,
			CachedVulkanPipeline& cachedPipeline);
	};
}