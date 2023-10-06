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

	RenderTask::RenderTask(uint64 rendererID, Ref<GraphicsSemaphore> renderCompletedSemaphore, Ref<GraphicsFence> renderCompletedFence) :
		RendererID(rendererID),
		RenderCompletedSemaphore(renderCompletedSemaphore),
		RenderCompletedFence(renderCompletedFence)
	{}
}