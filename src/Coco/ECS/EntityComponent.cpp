#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent(EntityID owner) : Owner(owner)
	{}
}