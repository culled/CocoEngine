#include "ECSpch.h"
#include "EntityComponent.h"

namespace Coco::ECS
{
	EntityComponent::EntityComponent(const Entity& owner) :
		_owner(owner)
	{}
}