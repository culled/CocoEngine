#include "Renderpch.h"
#include "RenderTask.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	RenderTask::RenderTask(uint64 id, uint64 rendererID, SharedRef<RenderView> view, uint64 pipelineID) :
		ID(id),
		RendererID(rendererID),
		View(view),
		PipelineID(pipelineID)
	{}
}