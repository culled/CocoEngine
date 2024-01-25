#pragma once

#include <Coco/Core/Types/Refs.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsSemaphore;
	class VulkanGraphicsFence;

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(VkCommandBuffer buffer, VkQueue queue);

		/// @brief Begins recording to this command buffer
		/// @param isSingleUse If true, this buffer will only be submitted once
		/// @param isSimultaneousUse If true, this buffer can be submitted to multiple queues
		void Begin(bool isSingleUse, bool isSimultaneousUse);
		void End();
		void Submit(
			std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores = std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>>(),
			std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores = std::span<Ref<VulkanGraphicsSemaphore>>(),
			Ref<VulkanGraphicsFence> signalFence = Ref<VulkanGraphicsFence>());
		void EndAndSubmit(
			std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores = std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>>(),
			std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores = std::span<Ref<VulkanGraphicsSemaphore>>(),
			Ref<VulkanGraphicsFence> signalFence = Ref<VulkanGraphicsFence>());
		void Reset();

		VkCommandBuffer GetCmdBuffer() const { return _commandBuffer; }

	private:
		VkCommandBuffer _commandBuffer;
		VkQueue _queue;
	};
}