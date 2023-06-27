#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent() : Owner(InvalidEntityID)
	{}

	EntityComponent::EntityComponent(EntityID owner) : Owner(owner)
	{}
}