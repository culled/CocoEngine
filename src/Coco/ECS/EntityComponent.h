#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"

namespace Coco::ECS
{

	struct COCOAPI EntityComponent
	{
		EntityID Owner;

		EntityComponent();
		EntityComponent(EntityID owner);
		virtual ~EntityComponent() = default;
	};
}