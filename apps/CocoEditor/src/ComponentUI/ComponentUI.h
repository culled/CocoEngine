#pragma once

#include <Coco/ECS/Entity.h>

namespace Coco
{
	struct ComponentUI
	{
		virtual ~ComponentUI() = default;
		virtual void DrawComponent(ECS::Entity& entity) = 0;
	};
}