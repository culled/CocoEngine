#pragma once

#include <Coco/Core/API.h>
#include "../Graphics/RenderView.h"
#include "../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Provides camera data to the renderer when rendering needs to occur
	class COCOAPI ICameraDataProvider
	{
	public:
		/// @brief Creates a render view that represents the camera settings of this provider
		/// @param pipeline The pipeline being used for rendering
		/// @param backbufferSize The size of the backbuffers
		/// @param backbuffers The backbuffers that will be rendered to
		/// @return A render view
		virtual Ref<RenderView> GetRenderView(
			const Ref<RenderPipeline>& pipeline, 
			const SizeInt& backbufferSize, 
			const List<WeakManagedRef<Image>>& backbuffers) = 0;
	};
}