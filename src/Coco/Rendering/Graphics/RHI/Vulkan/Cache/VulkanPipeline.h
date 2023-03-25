#pragma once

#include <Coco/Core/Resources/Resource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct ShaderRenderData;
	class RenderPipeline;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanRenderPass;
	class VulkanSubshader;

	/// @brief A cached Vulkan pipeline for a render pass and subshader combo
	class VulkanPipeline final : public CachedResource
	{
	public:
		/// @brief The index of the RenderPipeline pass that this pipeline was made for
		const uint SubpassIndex;

	private:
		GraphicsDeviceVulkan* _device;

		WeakRef<VulkanRenderPass> _renderPassRef;
		WeakRef<VulkanSubshader> _subshaderRef;

		VkPipelineLayout _pipelineLayout = nullptr;
		VkPipeline _pipeline = nullptr;

	public:
		VulkanPipeline(
			GraphicsDeviceVulkan* device, 
			const Ref<VulkanRenderPass>& renderPass, 
			const Ref<VulkanSubshader>& subshader, 
			uint subpassIndex);
		~VulkanPipeline() final;

		bool IsInvalid() const noexcept final { return _renderPassRef.expired() || _subshaderRef.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Updates this pipeline to reflect the current render pass and subshader
		/// @param globalDescriptorLayout The global descriptor layout
		void Update(const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Gets the underlying VkPipelineLayout
		/// @return The Vulkan pipeline layout
		VkPipelineLayout GetPipelineLayout() const noexcept { return _pipelineLayout; }

		/// @brief Gets the underlying VkPipeline
		/// @return The Vulkan pipeline
		VkPipeline GetPipeline() const noexcept { return _pipeline; }

	private:
		/// @brief Creates the pipeline layout and pipeline for the RenderPipeline and subshader combo
		/// @param globalDescriptorLayout The global descriptor layout
		void CreatePipeline(const VkDescriptorSetLayout& globalDescriptorLayout);

		/// @brief Destroys the pipeline layout and pipeline
		void DestroyPipeline() noexcept;
	};
}