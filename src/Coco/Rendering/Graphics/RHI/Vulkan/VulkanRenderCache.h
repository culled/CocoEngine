#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/Map.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	struct ShaderRenderData;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanSubshader;
	class VulkanRenderPass;
	class VulkanPipeline;

	/// @brief A cached shader resource
	struct CachedShader final : public CachedResource
	{
		/// @brief Cached subshaders of this shader
		UnorderedMap<string, Ref<VulkanSubshader>> Subshaders;

		CachedShader(const ShaderRenderData& shaderData);
		~CachedShader() final = default;

		bool IsInvalid() const noexcept final { return false; }
		bool NeedsUpdate() const noexcept final { return false; }
	};

	/// @brief A cache for global Vulkan objects (render passes, pipelines, and shaders)
	class VulkanRenderCache
	{
	private:
		static constexpr uint64_t s_staleTickCount = 500;

		GraphicsDeviceVulkan* _device;

		UnorderedMap<ResourceID, Ref<VulkanRenderPass>> _renderPassCache;
		UnorderedMap<uint64_t, Ref<VulkanPipeline>> _pipelineCache;
		UnorderedMap<ResourceID, Ref<CachedShader>> _shaderCache;

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
		Ref<VulkanRenderPass> GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline);

		/// @brief Gets or creates a pipeline for a render pass and shader combination
		/// @param renderPass The render pass
		/// @param subshader The subshader
		/// @param subpassIndex The index of the subpass in the pipeline
		/// @param globalDescriptorLayout The layout for the global descriptors
		/// @return The VulkanPipeline resource
		Ref<VulkanPipeline> GetOrCreatePipeline(
			const Ref<VulkanRenderPass>& renderPass,
			const Ref<VulkanSubshader>& subshader,
			uint32_t subpassIndex, 
			const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets or creates a Vulkan subshader for a shader's subshader
		/// @param subshaderName The name of the subshader
		/// @param shaderData The shader data
		/// @return The Vulkan subshader resource
		Ref<VulkanSubshader> GetOrCreateVulkanSubshader(const string& subshaderName, const ShaderRenderData& shaderData);
	};
}