#include "Renderpch.h"
#include "RenderTask.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	RenderTask::RenderTask() :
		RendererID(Math::MaxValue<uint64>()),
		RenderCompletedSemaphore(),
		RenderCompletedFence()
	{}

	RenderTask::RenderTask(
		uint64 rendererID, 
		const RenderContextRenderStats& stats, 
		Ref<GraphicsSemaphore> renderCompletedSemaphore, 
		Ref<GraphicsFence> renderCompletedFence) :
		RendererID(rendererID),
		RenderStats(stats),
		RenderCompletedSemaphore(renderCompletedSemaphore),
		RenderCompletedFence(renderCompletedFence)
	{}
}