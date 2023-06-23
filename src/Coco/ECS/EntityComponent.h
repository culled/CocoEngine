#pragma once

#include <Coco/Core/Core.h>
#include "EntityData.h"

namespace Coco::ECS
{
	struct COCOAPI EntityComponent
	{
		const EntityID Owner;

		EntityComponent(EntityID owner);
		virtual ~EntityComponent() = default;
	};
}