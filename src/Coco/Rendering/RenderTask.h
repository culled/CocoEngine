#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Graphics/GraphicsSemaphore.h"
#include "Graphics/GraphicsFence.h"
#include "Graphics/RenderContextTypes.h"

namespace Coco::Rendering
{
	/// @brief An object that can be used to synchronize multiple rendering operations
	struct RenderTask
	{
		/// @brief The ID of the renderer that started this task
		uint64 RendererID;

		/// @brief Stats for this specific render
		RenderContextRenderStats RenderStats;

		/// @brief The semaphore that will be signaled when this task completes
		Ref<GraphicsSemaphore> RenderCompletedSemaphore;

		/// @brief The fence that will be signaled when this task completes
		Ref<GraphicsFence> RenderCompletedFence;

		RenderTask();
		RenderTask(uint64 rendererID, const RenderContextRenderStats& stats, Ref<GraphicsSemaphore> renderCompletedSemaphore, Ref<GraphicsFence> renderCompletedFence);
	};
}