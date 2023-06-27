#pragma once

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Resources/ResourceCache.h>

#include "Resources/Cache/VulkanPipeline.h"
#include "Resources/Cache/VulkanRenderPass.h"
#include "Resources/Cache/VulkanShader.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	struct ShaderRenderData;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief A cache for global Vulkan objects (render passes, pipelines, and shaders)
	class VulkanRenderCache
	{
	private:
		GraphicsDeviceVulkan* _device;

		ManagedRef<ResourceCache<VulkanRenderPass>> _renderPassCache;
		ManagedRef<ResourceCache<VulkanPipeline>> _pipelineCache;
		ManagedRef<ResourceCache<VulkanShader>> _shaderCache;

	public:
		VulkanRenderCache(GraphicsDeviceVulkan* device);
		~VulkanRenderCache();

		/// @brief Clears all cached objects
		void Invalidate() noexcept;

		/// @brief Purges unused and invalid resources
		void PurgeResources() noexcept;

		/// @brief Gets or creates a render pass resource for a RenderPipeline
		/// @param renderPipeline The render pipeline
		/// @return The VulkanRenderPass resource
		VulkanRenderPass* GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline);

		/// @brief Gets or creates a pipeline for a render pass and shader combination
		/// @param renderPass The render pass
		/// @param subshader The subshader
		/// @param subpassIndex The index of the subpass in the pipeline
		/// @param globalDescriptorLayout The layout for the global descriptors
		/// @return The VulkanPipeline resource
		VulkanPipeline* GetOrCreatePipeline(
			VulkanRenderPass* renderPass,
			VulkanShader* shader,
			const string& subshaderName,
			uint32_t subpassIndex, 
			const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets or creates a Vulkan subshader for a shader's subshader
		/// @param subshaderName The name of the subshader
		/// @param shaderData The shader data
		/// @return The Vulkan subshader resource
		VulkanShader* GetOrCreateVulkanShader(const ShaderRenderData& shaderData);
	};
}