#pragma once

#include "../Entity.h"

namespace Coco::ECS
{
	/// @brief Base class for all components that can be attached to entities
	class EntityComponent
	{
	private:
		Entity _owner;

	protected:
		EntityComponent(const Entity& owner);

	public:
		virtual ~EntityComponent() = default;

		/// @brief Gets the entity that this component is attached to
		/// @return The owning entity
		const Entity& GetOwner() const { return _owner; }
	};
}