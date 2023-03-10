#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Rendering/Graphics/AttachmentDescription.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanShader.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDeviceVulkan;
	class RenderPipeline;
	class Shader;
	class RenderPipeline;

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
	/// Information for a Vulkan renderpass
	/// </summary>
	struct VulkanRenderPass
	{
		List<SubpassInfo> Subpasses;
		VkRenderPass RenderPass = nullptr;
	};

	/// <summary>
	/// Information for a Vulkan pipeline
	/// </summary>
	struct VulkanPipeline
	{
		static const VulkanPipeline None;

		VkPipelineLayout Layout = nullptr;
		VkPipeline Pipeline = nullptr;
	};

	/// <summary>
	/// A cache for Vulkan pipeline objects
	/// </summary>
	class VulkanRenderCache
	{
	private:
		GraphicsDeviceVulkan* _device;

		std::hash<const RenderPipeline*> _renderPipelineHasher;
		std::hash<VkRenderPass> _renderPassHasher;

		Map<uint64_t, VulkanRenderPass> _renderPassCache;
		Map<uint64_t, VulkanPipeline> _pipelineCache;
		Map<ResourceID, GraphicsResourceRef<VulkanShader>> _shaderCache;

	public:
		VulkanRenderCache(GraphicsDeviceVulkan* device);
		~VulkanRenderCache();

		/// <summary>
		/// Clears all cached objects
		/// </summary>
		void Invalidate() noexcept;

		/// <summary>
		/// Gets or creates a VulkanRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		/// <returns>The VulkanRenderPass for the pipeline</returns>
		VulkanRenderPass GetOrCreateRenderPass(RenderPipeline* renderPipeline);

		/// <summary>
		/// Gets or creates a VulkanPipeline for a render pass and shader combination
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="subpassName">The name of the subpass</param>
		/// <param name="subpassIndex">The index of the subpass in the pipeline</param>
		/// <param name="shader">The shader</param>
		/// <returns>The VulkanPipeline for the shader and render pass</returns>
		VulkanPipeline GetOrCreatePipeline(
			VulkanRenderPass renderPass, 
			const string& subpassName,
			uint32_t subpassIndex, 
			const Shader* shader,
			VkDescriptorSetLayout globalDescriptorLayout);

		/// <summary>
		/// Gets or creates a VulkanShader for a shader
		/// </summary>
		/// <param name="shader">The shader</param>
		/// <returns>The Vulkan-ready shader</returns>
		GraphicsResourceRef<VulkanShader> GetOrCreateVulkanShader(const Shader* shader);

	private:
		/// <summary>
		/// Creates a VulkanRenderPass for a RenderPipeline
		/// </summary>
		/// <param name="renderPipeline">The render pipeline</param>
		/// <returns>The VulkanRenderPass for the pipeline</returns>
		VulkanRenderPass CreateRenderPass(RenderPipeline* renderPipeline);

		/// <summary>
		/// Creates a VulkanPipeline for a render pass and shader combination
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="subpassName">The name of the subpass</param>
		/// <param name="subpassIndex">The index of the subpass in the pipeline</param>
		/// <param name="shader">The shader</param>
		/// <returns>The VulkanPipeline for the shader and render pass</returns>
		VulkanPipeline CreatePipeline(
			VulkanRenderPass renderPass, 
			const string& subpassName, 
			uint32_t subpassIndex, 
			const Shader* shader,
			VkDescriptorSetLayout globalDescriptorLayout);
	};
}