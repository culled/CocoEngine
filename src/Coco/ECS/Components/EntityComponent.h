#pragma once

#include "../Entity.h"

namespace Coco::ECS
{
	/// @brief Base class for all components that can be attached to entities
	class EntityComponent
	{
	public:
		virtual ~EntityComponent() = default;

		virtual const char* GetComponentTypename() const = 0;

		/// @brief Gets the entity that this component is attached to
		/// @return The owning entity
		const Entity& GetOwner() const { return _owner; }
		Entity& GetOwner() { return _owner; }

	protected:
		EntityComponent(const Entity& owner);

	private:
		Entity _owner;
	};
}