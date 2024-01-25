#pragma once

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/EntityInfoComponent.h>

namespace Coco
{
	class EntityInfoComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::EntityInfoComponent& entityInfo);
	};
}