#include "ECSpch.h"
#include "Rigidbody3DComponentSerializer.h"

#include "../../../Components/Physics3D/Rigidbody3DComponent.h"
#include <yaml-cpp/yaml.h>

namespace Coco::ECS
{
	void Rigidbody3DComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const Rigidbody3DComponent& body = entity.GetComponent<Rigidbody3DComponent>();

		emitter << YAML::Key << "mass" << YAML::Value << body._mass;
		emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(body._type);
	}

	void Rigidbody3DComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		using namespace Physics3D;

		double mass = baseNode["mass"].as<double>();
		Rigidbody3DType type = static_cast<Rigidbody3DType>(baseNode["type"].as<int>());

		entity.AddComponent<Rigidbody3DComponent>(mass, type);
	}
}