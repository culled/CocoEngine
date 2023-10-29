#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Graphics/RenderView.h"
#include "Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief A task that defines a rendering operation
	struct RenderTask
	{
		/// @brief The ID of this task
		uint64 ID;

		/// @brief The ID of the renderer that started this task
		uint64 RendererID;

		/// @brief The view that will be rendered
		SharedRef<RenderView> View;

		/// @brief The id of the pipeline that will be used for rendering
		uint64 PipelineID;

		RenderTask(uint64 id, uint64 rendererID, SharedRef<RenderView> view, uint64 pipelineID);
	};
}