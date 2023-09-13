#pragma once

#include "GraphicsFence.h"
#include "GraphicsSemaphore.h"

namespace Coco::Rendering
{
	/// @brief A context that performs rendering operations
	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		/// @brief Blocks until this context is idle and ready to be rendered to
		virtual void WaitForRenderingToComplete() = 0;

		/// @brief Gets the semaphore used to wait until an image becomes available for rendering
		/// @return The semaphore
		virtual GraphicsSemaphore* GetImageAvailableSemaphore() = 0;

		/// @brief Gets the semaphore used to wait until rendering with this context has completed
		/// @return The semaphore
		virtual GraphicsSemaphore* GetRenderCompletedSemaphore() = 0;

		/// @brief Gets the fence used to wait until rendering with this context has completed
		/// @return The fence
		virtual GraphicsFence* GetRenderCompletedFence() = 0;

		/// @brief Resets this context
		void Reset();

	protected:
		/// @brief Called when this context is reset
		virtual void ResetImpl() = 0;
	};
}