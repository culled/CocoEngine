#include "ECSpch.h"
#include "EntityInfoComponentSerializer.h"

#include "../../Components/EntityInfoComponent.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void EntityInfoComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		emitter << YAML::Key << "name" << YAML::Value << info._name;
		emitter << YAML::Key << "id" << YAML::Value << info._entityID;
		emitter << YAML::Key << "isActive" << YAML::Value << info._isActive;
	}

	void EntityInfoComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		info._name = baseNode["name"].as<string>();
		info._entityID = baseNode["id"].as<uint64>();
		info._isActive = baseNode["isActive"].as<bool>();
	}
}