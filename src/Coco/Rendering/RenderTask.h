#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Graphics/GraphicsSemaphore.h"
#include "Graphics/GraphicsFence.h"

namespace Coco::Rendering
{
	/// @brief An object that can be used to synchronize multiple rendering operations
	struct RenderTask
	{
		/// @brief The semaphore that will be signaled when this task completes
		Ref<GraphicsSemaphore> RenderCompletedSemaphore;

		/// @brief The fence that will be signaled when this task completes
		Ref<GraphicsFence> RenderCompletedFence;

		RenderTask();
		RenderTask(Ref<GraphicsSemaphore> renderCompletedSemaphore, Ref<GraphicsFence> renderCompletedFence);
	};
}