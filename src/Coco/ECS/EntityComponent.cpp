#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent() : _owner(InvalidEntityID)
	{}

	EntityComponent::EntityComponent(EntityID owner) : _owner(owner)
	{}
}