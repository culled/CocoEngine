#pragma once

#include "../Graphics/RenderView.h"

namespace Coco::Rendering
{
	/// @brief Provides scene data to a RenderView
	class SceneDataProvider
	{
	public:
		/// @brief Called when the given RenderView needs scene data to be filled in
		/// @param renderView The render view
		virtual void GatherSceneData(RenderView& renderView) = 0;
	};
}