#include "ECSpch.h"
#include "Transform3DComponentSerializer.h"

#include "../../Components/Transform3DComponent.h"

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::ECS
{
	void Transform3DComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();

		emitter << YAML::Key << "localPosition" << YAML::Value << transform.Transform.LocalPosition;
		emitter << YAML::Key << "localRotation" << YAML::Value << transform.Transform.LocalRotation;
		emitter << YAML::Key << "localScale" << YAML::Value << transform.Transform.LocalScale;
		emitter << YAML::Key << "inheritParentTransform" << YAML::Value << transform.InheritParentTransform;
	}

	void Transform3DComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		Vector3 position = baseNode["localPosition"].as<Vector3>();
		Quaternion rotation = baseNode["localRotation"].as<Quaternion>();
		Vector3 scale = baseNode["localScale"].as<Vector3>();
		bool inherit = baseNode["inheritParentTransform"].as<bool>();

		entity.AddComponent<Transform3DComponent>(position, rotation, scale, inherit);
	}
}