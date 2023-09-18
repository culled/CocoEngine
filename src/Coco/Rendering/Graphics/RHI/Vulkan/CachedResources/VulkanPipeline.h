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

		static GraphicsDeviceResourceID MakeKey(
			const VulkanRenderPass& renderPass,
			const VulkanRenderPassShader& shader,
			uint32 subpassIndex);

		VkPipeline GetPipeline() const { return _pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;
	};
}