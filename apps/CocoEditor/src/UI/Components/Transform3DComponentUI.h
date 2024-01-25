#pragma once

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/Transform3DComponent.h>

namespace Coco
{
	class Transform3DComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::Transform3DComponent& transform);
	};
}