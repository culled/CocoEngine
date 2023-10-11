#include "ECSpch.h"
#include "EntityInfoComponent.h"
#include <Coco/Core/Math/Math.h>
#include "../Entity.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent() :
		EntityInfoComponent("Entity", Math::MaxValue<uint64>(), std::optional<Entity>())
	{}

	EntityInfoComponent::EntityInfoComponent(const string& name, const EntityID& id, std::optional<Entity> parent) :
		Name(name),
		ID(id),
		Parent(parent),
		IsActive(true)
	{}
}