#include "Renderpch.h"
#include "CompiledRenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	CompiledRenderPipeline::CompiledRenderPipeline(uint64 pipelineID) :
		PipelineID(pipelineID),
		Version(0),
		RenderPasses{},
		InputAttachments{},
		SupportsMSAA(false)
	{}
}