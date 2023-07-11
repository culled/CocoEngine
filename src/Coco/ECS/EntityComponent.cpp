#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent(EntityID owner) : _owner(owner)
	{}
}