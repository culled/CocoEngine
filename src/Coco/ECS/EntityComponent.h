#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"

namespace Coco::ECS
{
	class COCOAPI EntityComponent
	{
	protected:
		EntityID _owner;

	public:
		EntityComponent();
		EntityComponent(EntityID owner);
		virtual ~EntityComponent() = default;
	};
}