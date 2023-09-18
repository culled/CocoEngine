#pragma once

#include "../Graphics/RenderView.h"

namespace Coco::Rendering
{
	class SceneDataProvider
	{
	public:
		virtual void GatherSceneData(RenderView& renderView) = 0;
	};
}