#pragma once
#include "../../../GraphicsDeviceResource.h"
#include "../VulkanIncludes.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderPassShader.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A Vulkan framebuffer
	class VulkanPipeline : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		uint64 _version;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;
		uint32 _subpassIndex;
		double _lastUsedTime;

	public:
		VulkanPipeline(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		~VulkanPipeline();

		/// @brief Makes a key from a render pass, shader, and subpass index
		/// @param renderPass The render pass
		/// @param shader The shader
		/// @param subpassIndex The subpass index
		/// @return A key created from the given items
		static GraphicsDeviceResourceID MakeKey(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		/// @brief Gets this pipeline's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		VkPipeline GetPipeline() const { return _pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

		bool NeedsUpdate(const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader);

		void Update(const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;

	private:
		static uint64 MakeVersion(const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader);

		void CreatePipeline(const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);
		void DestroyPipeline();
	};
}