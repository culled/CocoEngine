#pragma once

#include "../Graphics/RenderView.h"
#include "../Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Sets up a RenderView that can be used for rendering
	class RenderViewProvider
	{
	public:
		/// @brief Called when a RenderView should be setup
		/// @param renderView The render view to setup
		/// @param pipeline The pipeline being used to render
		/// @param rendererID The ID of the renderer
		/// @param backbufferSize The size of the backbuffers
		/// @param backbuffers The backbuffers being rendered to, if any
		/// @return A render view
		virtual void SetupRenderView(
			RenderView& renderView,
			const CompiledRenderPipeline& pipeline, 
			uint64 rendererID,
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) = 0;
	};
}