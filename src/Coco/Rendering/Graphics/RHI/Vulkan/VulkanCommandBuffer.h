#pragma once

#include "../../../Renderpch.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A buffer than rendering commands can be written to
	class VulkanCommandBuffer
	{
		friend class VulkanCommandBufferPool;

	public:
		/// @brief States of a command buffer
		enum class State
		{
			Ready,
			Recording,
			RecordingEnded,
			Submitted
		};

	private:
		State _currentState;
		VkCommandBuffer _commandBuffer;
		VkQueue _queue;

	public:
		VulkanCommandBuffer(VkCommandBuffer buffer, VkQueue queue);
		~VulkanCommandBuffer() = default;

		/// @brief Begins recording to this command buffer
		/// @param isSingleUse If true, this buffer will only be submitted once
		/// @param isSimultaneousUse If true, this buffer can be submitted to multiple queues
		void Begin(bool isSingleUse, bool isSimultaneousUse);

		/// @brief Ends recording to this command buffer
		void End();

		/// @brief Submits this command buffer
		/// @param waitSemaphores Semaphores to wait on before performing this buffer's work
		/// @param signalSemaphores Semaphores to signal once this buffer's work has been completed
		/// @param signalFence A fence to signal once this buffer's work has been completed
		void Submit(
			std::vector<VulkanGraphicsSemaphore*>* waitSemaphores = nullptr,
			std::vector<VulkanGraphicsSemaphore*>* signalSemaphores = nullptr,
			VulkanGraphicsFence* signalFence = nullptr);

		/// @brief Ends and submits this buffer
		/// @param waitSemaphores Semaphores to wait on before performing this buffer's work
		/// @param signalSemaphores Semaphores to signal once this buffer's work has been completed
		/// @param signalFence A fence to signal once this buffer's work has been completed
		void EndAndSubmit(
			std::vector<VulkanGraphicsSemaphore*>* waitSemaphores = nullptr,
			std::vector<VulkanGraphicsSemaphore*>* signalSemaphores = nullptr,
			VulkanGraphicsFence* signalFence = nullptr);

		/// @brief Resets this command buffer
		void Reset();

		/// @brief Gets the Vulkan command buffer
		/// @return The Vulkan command buffer
		VkCommandBuffer GetCmdBuffer() const { return _commandBuffer; }
	};
}