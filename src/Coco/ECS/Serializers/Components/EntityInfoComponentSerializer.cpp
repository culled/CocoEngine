#include "ECSpch.h"
#include "EntityInfoComponentSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void EntityInfoComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		emitter << YAML::Key << "isSelfActive" << YAML::Value << info._isSelfActive;
	}

	bool EntityInfoComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		info._isSelfActive = baseNode["isSelfActive"].as<bool>();

		return true;
	}
}