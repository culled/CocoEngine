#include "ECSpch.h"
#include "SphereColliderComponentSerializer.h"

#include "../../../Components/Physics3D/SphereColliderComponent.h"

#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void SphereColliderComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const SphereColliderComponent& collider = entity.GetComponent<SphereColliderComponent>();

		emitter << YAML::Key << "radius" << YAML::Value << collider._radius;
	}

	void SphereColliderComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		double radius = baseNode["radius"].as<double>();

		entity.AddComponent<SphereColliderComponent>(radius);
	}
}