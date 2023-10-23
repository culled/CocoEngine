#include "ECSpch.h"
#include "EntityInfoComponent.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent() :
		EntityInfoComponent("Entity", InvalidEntityID)
	{}

	EntityInfoComponent::EntityInfoComponent(const string& name, const EntityID& id) :
		Name(name),
		ID(id),
		IsActive(true)
	{}
}