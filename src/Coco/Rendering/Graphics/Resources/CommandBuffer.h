#pragma once

#include "GraphicsResource.h"

#include <Coco/Core/Types/List.h>

namespace Coco::Rendering
{
	class IGraphicsSemaphore;
	class IGraphicsFence;

	/// @brief A buffer that can record commands
	class COCOAPI CommandBuffer : public IGraphicsResource
	{
	public:
		/// @brief States of a command buffer
		enum class CommandBufferState
		{
			Ready,
			Recording,
			RecordingEnded,
			Submitted
		};

		/// @brief If true, this is a top-level buffer
		const bool IsPrimary;

	protected:
		/// @brief The current state of this buffer
		CommandBufferState CurrentState;

	protected:
		CommandBuffer(bool isPrimary) noexcept;

	public:
		virtual ~CommandBuffer() = default;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&) = delete;

		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;

		/// @brief Begins recording to this command buffer
		/// @param isSingleUse If true, this buffer will only be submitted once
		/// @param isSimultaneousUse If true, this buffer can be submitted to multiple queues
		virtual void Begin(bool isSingleUse, bool isSimultaneousUse);

		/// @brief Ends recording to this command buffer
		virtual void End();

		/// @brief Submits this command buffer
		/// @param waitSemaphores Semaphores to wait on before performing this buffer's work
		/// @param signalSemaphores Semaphores to signal once this buffer's work has been completed
		/// @param signalFence A fence to signal once this buffer's work has been completed
		virtual void Submit(
			const List<IGraphicsSemaphore*>& waitSemaphores = {},
			const List<IGraphicsSemaphore*>& signalSemaphores = {},
			IGraphicsFence* signalFence = nullptr);

		/// @brief Resets this command buffer
		virtual void Reset();

		/// @brief Ends and submits this buffer
		/// @param waitSemaphores Semaphores to wait on before performing this buffer's work
		/// @param signalSemaphores Semaphores to signal once this buffer's work has been completed
		/// @param signalFence A fence to signal once this buffer's work has been completed
		void EndAndSubmit(
			const List<IGraphicsSemaphore*>& waitSemaphores = {},
			const List<IGraphicsSemaphore*>& signalSemaphores = {},
			IGraphicsFence* signalFence = nullptr);

	protected:
		/// @brief Implementation to begin recording this command buffer
		/// @param isSingleUse If true, this buffer will only be submitted once
		/// @param isSimultaneousUse If true, this buffer can be submitted to multiple queues
		virtual void BeginImpl(bool isSingleUse, bool isSimultaneousUse) = 0;

		/// @brief Implementation to end recording this command buffer
		virtual void EndImpl() = 0;

		/// @brief Implementation to submit this command buffer
		/// @param waitSemaphores Semaphores to wait on before performing this buffer's work
		/// @param signalSemaphores Semaphores to signal once this buffer's work has been completed
		/// @param signalFence A fence to signal once this buffer's work has been completed
		virtual void SubmitImpl(
			const List<IGraphicsSemaphore*>& waitSemaphores,
			const List<IGraphicsSemaphore*>& signalSemaphores,
			IGraphicsFence* signalFence) = 0;

		/// @brief Implementation to reset this command buffer
		virtual void ResetImpl() = 0;
	};
}