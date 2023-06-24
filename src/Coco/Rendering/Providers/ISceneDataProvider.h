#pragma once

#include <Coco/Core/API.h>
#include "../Graphics/RenderView.h"

namespace Coco::Rendering
{
	/// @brief Provides scene data to the renderer when a render occurs
	class COCOAPI ISceneDataProvider
	{
	public:
		/// @brief Fills the given render view with data for geometry that should be rendered
		/// @param renderView The RenderView
		virtual void GetSceneData(Ref<RenderView>& renderView) = 0;
	};
}