#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"

namespace Coco::ECS
{
	/// @brief Base class for a component that can be attached to an entity
	class COCOAPI EntityComponent
	{	
	public:
		/// @brief The ID of the entity that this component is attached to
		const EntityID Owner;

	public:
		EntityComponent(const EntityID& owner);
		EntityComponent(EntityComponent&& other);
		virtual ~EntityComponent() = default;
	};
}