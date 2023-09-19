#include "Renderpch.h"
#include "RenderTask.h"

namespace Coco::Rendering
{
	RenderTask::RenderTask() :
		RenderCompletedSemaphore(),
		RenderCompletedFence()
	{}

	RenderTask::RenderTask(Ref<GraphicsSemaphore> renderCompletedSemaphore, Ref<GraphicsFence> renderCompletedFence) :
		RenderCompletedSemaphore(renderCompletedSemaphore),
		RenderCompletedFence(renderCompletedFence)
	{}
}