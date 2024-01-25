#include "ECSpch.h"
#include "BoxColliderComponentSerializer.h"

#include "../../../Components/Physics3D/BoxColliderComponent.h"

#include <Coco/Third Party/yaml/Converters.h>
#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void BoxColliderComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const BoxColliderComponent& collider = entity.GetComponent<BoxColliderComponent>();

		emitter << YAML::Key << "size" << YAML::Value << collider._size;
	}

	void BoxColliderComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		Vector3 size = baseNode["size"].as<Vector3>();

		entity.AddComponent<BoxColliderComponent>(size);
	}
}