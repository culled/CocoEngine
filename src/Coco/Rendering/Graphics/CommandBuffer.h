#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include "GraphicsSemaphore.h"
#include "GraphicsFence.h"

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
		enum class State
		{
			Ready,
			Recording,
			RecordingEnded,
			Submitted,
			NotAllocated
		};

	protected:
		State CurrentState;
		bool IsPrimary;

	protected:
		CommandBuffer(bool isPrimary);

	public:
		virtual ~CommandBuffer();

		/// <summary>
		/// Begins recording this buffer
		/// </summary>
		/// <param name="isSingleUse">If true, this buffer will only be submitted once</param>
		/// <param name="isSimultaneousUse">If true, this buffer can be submitted to multiple queues</param>
		virtual void Begin(bool isSingleUse, bool isSimultaneousUse) = 0;

		/// <summary>
		/// Ends recording this buffer
		/// </summary>
		virtual void End() = 0;

		/// <summary>
		/// Submits this buffer
		/// </summary>
		/// <param name="waitSemaphores">Semaphores to wait on before performing this buffer's work</param>
		/// <param name="signalSemaphores">Semaphores to signal once this buffer's work has been completed</param>
		/// <param name="signalFence">A fence to signal once this buffer's work has been completed</param>
		virtual void Submit(const List<GraphicsSemaphore*>& waitSemaphores = {}, const List<GraphicsSemaphore*>& signalSemaphores = {}, GraphicsFence* signalFence = nullptr) = 0;

		/// <summary>
		/// Resets this buffer
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Ends and submits this buffer
		/// </summary>
		/// <param name="waitSemaphores">Semaphores to wait on before performing this buffer's work</param>
		/// <param name="signalSemaphores">Semaphores to signal once this buffer's work has been completed</param>
		/// <param name="signalFence">A fence to signal once this buffer's work has been completed</param>
		void EndAndSubmit(const List<GraphicsSemaphore*>& waitSemaphores = {}, const List<GraphicsSemaphore*>& signalSemaphores = {}, GraphicsFence* signalFence = nullptr);
	};
}