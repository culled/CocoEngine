#include "ECSpch.h"

#include "ComponentSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void ComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		emitter << YAML::Key << GetSectionName() << YAML::Value << YAML::BeginMap;

		SerializeImpl(emitter, entity);

		emitter << YAML::EndMap;
	}

	bool ComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		if (baseNode[GetSectionName()])
		{
			DeserializeImpl(baseNode[GetSectionName()], entity);
			return true;
		}

		return false;
	}
}