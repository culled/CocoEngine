#include "ECSpch.h"
#include "ColliderComponent.h"

namespace Coco::ECS
{
	ColliderComponent::ColliderComponent(const Entity& owner) :
		EntityComponent(owner)
	{}
}