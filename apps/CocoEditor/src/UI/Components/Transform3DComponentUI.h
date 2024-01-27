#pragma once

#include <Coco/ECS/Components/Transform3DComponent.h>

namespace Coco
{
	class ViewportPanel;

	class Transform3DComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::Transform3DComponent& transform);
		static void DrawViewport2D(ViewportPanel& viewport);
	};
}