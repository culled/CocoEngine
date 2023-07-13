#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent(const EntityID& owner) : Owner(owner)
	{}

	EntityComponent::EntityComponent(EntityComponent && other) :
		Owner(other.Owner)
	{}
}