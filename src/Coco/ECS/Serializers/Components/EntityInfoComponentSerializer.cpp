#include "ECSpch.h"
#include "EntityInfoComponentSerializer.h"

#include "../../Components/EntityInfoComponent.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void EntityInfoComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		emitter << YAML::Key << "name" << YAML::Value << info.Name;
		emitter << YAML::Key << "id" << YAML::Value << info.ID;
		emitter << YAML::Key << "isActive" << YAML::Value << info.IsActive;

		if (info.Parent.has_value())
		{
			const EntityInfoComponent& parentInfo = info.Parent->GetComponent<EntityInfoComponent>();
			emitter << YAML::Key << "parent" << YAML::Value << parentInfo.ID;
		}
	}

	void EntityInfoComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		info.Name = baseNode["name"].as<string>();
		info.ID = baseNode["id"].as<uint64>();
		info.IsActive = baseNode["isActive"].as<bool>();

		if (baseNode["parent"])
		{
			// TODO: parenting
		}
	}
}