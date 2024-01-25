#include "ECSpch.h"
#include "EntityInfoComponentSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void EntityInfoComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();
	}

	bool EntityInfoComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		return true;
	}
}