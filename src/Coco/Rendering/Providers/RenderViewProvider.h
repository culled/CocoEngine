#pragma once

#include "../Graphics/RenderView.h"
#include "../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Provides a RenderView that can be used for rendering
	class RenderViewProvider
	{
	public:
		/// @brief Called when a RenderView should be created
		/// @param pipeline The pipeline being used to render
		/// @param backbufferSize The size of the backbuffers
		/// @param backbuffers The backbuffers being rendered to, if any
		/// @return A render view
		virtual UniqueRef<RenderView> CreateRenderView(
			const RenderPipeline& pipeline, 
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) = 0;
	};
}