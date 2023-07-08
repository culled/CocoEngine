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

	/// @brief A cached Vulkan pipeline for a render pass and subshader combo
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
			const VulkanRenderPass* renderPass, 
			const VulkanShader* shader,
			const string& subshaderName,
			uint subpassIndex);
		~VulkanPipeline() final;

		DefineResourceType(VulkanPipeline)

		static ResourceID GetResourceID(const VulkanRenderPass* renderPass, const VulkanShader* shader);
		static ResourceVersion GetResourceVersion(const VulkanRenderPass* renderPass, const VulkanShader* shader);

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }
		bool NeedsUpdate(const VulkanRenderPass* renderPass, const VulkanShader* shader) const noexcept;

		/// @brief Updates this pipeline to reflect the current render pass and subshader
		/// @param globalDescriptorLayout The global descriptor layout
		void Update(VulkanRenderPass* renderPass, VulkanShader* shader, const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets the underlying VkPipelineLayout
		/// @return The Vulkan pipeline layout
		VkPipelineLayout GetPipelineLayout() const noexcept { return _pipelineLayout; }

		/// @brief Gets the underlying VkPipeline
		/// @return The Vulkan pipeline
		VkPipeline GetPipeline() const noexcept { return _pipeline; }

	private:
		/// @brief Creates the pipeline layout and pipeline for the RenderPipeline and subshader combo
		/// @param globalDescriptorLayout The global descriptor layout
		void CreatePipeline(VulkanRenderPass* renderPass, VulkanShader* shader, const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Destroys the pipeline layout and pipeline
		void DestroyPipeline() noexcept;
	};
}