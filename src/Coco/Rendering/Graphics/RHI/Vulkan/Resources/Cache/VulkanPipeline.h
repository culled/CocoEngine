#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include "../../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct ShaderRenderData;
	class RenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanRenderPass;
	class VulkanShader;

	/// @brief A cached Vulkan pipeline for a render pass and shader combo
	class VulkanPipeline final : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
	private:
		string _subshaderName;
		uint _subpassIndex;

		VkPipelineLayout _pipelineLayout = nullptr;
		VkPipeline _pipeline = nullptr;

	public:
		VulkanPipeline(
			ResourceID id,
			const string& name,
			const VulkanRenderPass& renderPass, 
			const VulkanShader& shader,
			const string& subshaderName,
			uint subpassIndex);
		~VulkanPipeline() final;

		DefineResourceType(VulkanPipeline)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }

		/// @brief Gets an id derived from a render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return A combined ID
		static ResourceID GetResourceID(const VulkanRenderPass& renderPass, const VulkanShader& shader);

		/// @brief Gets a version derived from a render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return A combined version
		static ResourceVersion GetResourceVersion(const VulkanRenderPass& renderPass, const VulkanShader& shader);

		/// @brief Determines if this pipeline needs to be updated based on the given render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return True if this cached resource should be updated
		bool NeedsUpdate(const VulkanRenderPass& renderPass, const VulkanShader& shader) const noexcept;

		/// @brief Updates this pipeline to reflect the current render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param globalDescriptorLayout The global descriptor layout
		void Update(VulkanRenderPass& renderPass, const VulkanShader& shader, const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets the underlying VkPipelineLayout
		/// @return The Vulkan pipeline layout
		VkPipelineLayout GetPipelineLayout() noexcept { return _pipelineLayout; }

		/// @brief Gets the underlying VkPipeline
		/// @return The Vulkan pipeline
		VkPipeline GetPipeline() noexcept { return _pipeline; }

	private:
		/// @brief Creates the pipeline layout and pipeline for the RenderPipeline and subshader combo
		/// @param globalDescriptorLayout The global descriptor layout
		void CreatePipeline(VulkanRenderPass& renderPass, const VulkanShader& shader, const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Destroys the pipeline layout and pipeline
		void DestroyPipeline() noexcept;
	};
}