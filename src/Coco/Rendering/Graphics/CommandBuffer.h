#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "IGraphicsSemaphore.h"
#include "IGraphicsFence.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A buffer that can record commands
	/// </summary>
	class COCOAPI CommandBuffer
	{
	public:
		/// <summary>
		/// States of a command buffer
		/// </summary>
		enum class CommandBufferState
		{
			Ready,
			Recording,
			RecordingEnded,
			Submitted
		};

	protected:
		CommandBufferState CurrentState;
		bool IsPrimary;

	protected:
		CommandBuffer(bool isPrimary) noexcept;

	public:
		virtual ~CommandBuffer() = default;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&) = delete;

		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;

		/// <summary>
		/// Begins recording to this command buffer
		/// </summary>
		/// <param name="isSingleUse">If true, this buffer will only be submitted once</param>
		/// <param name="isSimultaneousUse">If true, this buffer can be submitted to multiple queues</param>
		virtual void Begin(bool isSingleUse, bool isSimultaneousUse);

		/// <summary>
		/// Ends recording to this command buffer
		/// </summary>
		virtual void End();

		/// <summary>
		/// Submits this command buffer
		/// </summary>
		/// <param name="waitSemaphores">Semaphores to wait on before performing this buffer's work</param>
		/// <param name="signalSemaphores">Semaphores to signal once this buffer's work has been completed</param>
		/// <param name="signalFence">A fence to signal once this buffer's work has been completed</param>
		virtual void Submit(
			const List<IGraphicsSemaphore*>& waitSemaphores = {},
			const List<IGraphicsSemaphore*>& signalSemaphores = {},
			IGraphicsFence* signalFence = nullptr);

		/// <summary>
		/// Resets this command buffer
		/// </summary>
		virtual void Reset();

		/// <summary>
		/// Ends and submits this buffer
		/// </summary>
		/// <param name="waitSemaphores">Semaphores to wait on before performing this buffer's work</param>
		/// <param name="signalSemaphores">Semaphores to signal once this buffer's work has been completed</param>
		/// <param name="signalFence">A fence to signal once this buffer's work has been completed</param>
		void EndAndSubmit(
			const List<IGraphicsSemaphore*>& waitSemaphores = {},
			const List<IGraphicsSemaphore*>& signalSemaphores = {},
			IGraphicsFence* signalFence = nullptr);

	protected:
		/// <summary>
		/// Implementation to begin recording this command buffer
		/// </summary>
		/// <param name="isSingleUse">If true, this buffer will only be submitted once</param>
		/// <param name="isSimultaneousUse">If true, this buffer can be submitted to multiple queues</param>
		virtual void BeginImpl(bool isSingleUse, bool isSimultaneousUse) = 0;

		/// <summary>
		/// Implementation to end recording this command buffer
		/// </summary>
		virtual void EndImpl() = 0;

		/// <summary>
		/// Implementation to submit this command buffer
		/// </summary>
		/// <param name="waitSemaphores">Semaphores to wait on before performing this buffer's work</param>
		/// <param name="signalSemaphores">Semaphores to signal once this buffer's work has been completed</param>
		/// <param name="signalFence">A fence to signal once this buffer's work has been completed</param>
		virtual void SubmitImpl(
			const List<IGraphicsSemaphore*>& waitSemaphores,
			const List<IGraphicsSemaphore*>& signalSemaphores,
			IGraphicsFence* signalFence) = 0;

		/// <summary>
		/// Implementation to reset this command buffer
		/// </summary>
		virtual void ResetImpl() = 0;
	};
}