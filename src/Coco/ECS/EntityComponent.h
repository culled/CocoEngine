#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"

namespace Coco::ECS
{
	class COCOAPI EntityComponent
	{	
	public:
		const EntityID Owner;

	public:
		EntityComponent(const EntityID& owner);
		EntityComponent(EntityComponent&& other);
		virtual ~EntityComponent() = default;
	};
}