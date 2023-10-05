#include "ECSpch.h"
#include "EntityInfoComponent.h"
#include "../Entity.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent(const string& name, std::optional<Entity> parent) :
		Name(name),
		Parent(parent),
		IsActive(true)
	{}
}