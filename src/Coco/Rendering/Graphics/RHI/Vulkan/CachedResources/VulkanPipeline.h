#pragma once
#include "CachedVulkanResource.h"
#include "../../../ShaderUniformLayout.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;
	class VulkanRenderPass;
	class VulkanShader;
	struct VulkanDescriptorSetLayout;

	/// @brief A Vulkan pipeline
	class VulkanPipeline : 
		public CachedVulkanResource
	{
		friend class VulkanGraphicsDeviceCache;

	private:
		static VulkanDescriptorSetLayout _sEmptyLayout;

		uint64 _version;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
		uint32 _subpassIndex;

	public:
		VulkanPipeline(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader,
			uint32 subpassIndex,
			const VulkanDescriptorSetLayout* globalLayout);

		~VulkanPipeline();

		/// @brief Makes a key from a render pass, shader, and subpass index
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The subpass index
		/// @param globalLayout The global descriptor set layout, if one exists
		/// @return A key created from the given items
		static GraphicsDeviceResourceID MakeKey(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader,
			uint32 subpassIndex,
			const VulkanDescriptorSetLayout* globalLayout);

		/// @brief Gets this pipeline's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan pipeline
		/// @return The Vulkan pipeline
		VkPipeline GetPipeline() const { return _pipeline; }

		/// @brief Gets the Vulkan pipeline layout
		/// @return The Vulkan pipeline layout
		VkPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

		/// @brief Determines if this pipeline needs to be updated
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return True if this pipeline should be updated
		bool NeedsUpdate(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader);

		/// @brief Updates this pipeline
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The index of this pipeline within the RenderPipeline
		/// @param globalLayout The global descriptor layout, if one exists
		void Update(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader,
			uint32 subpassIndex,
			const VulkanDescriptorSetLayout* globalLayout);

	private:
		/// @brief Creates a version from a render pass and shader
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @return A combined version
		static uint64 MakeVersion(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader);

		/// @brief Creates the pipeline
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The index of this pipeline within the RenderPipeline
		/// @param globalLayout The global descriptor layout, if one exists
		void CreatePipeline(
			const VulkanRenderPass& renderPass,
			const VulkanShader& shader,
			uint32 subpassIndex,
			const VulkanDescriptorSetLayout* globalLayout);

		/// @brief Destroys the pipeline
		void DestroyPipeline();

		/// @brief Creates the empty descriptor set layout
		/// @param device The device
		static void CreateEmptyLayout(VulkanGraphicsDevice& device);

		/// @brief Destroys the empty descriptor set layout
		/// @param device The device
		static void DestroyEmptyLayout(VulkanGraphicsDevice& device);
	};
}