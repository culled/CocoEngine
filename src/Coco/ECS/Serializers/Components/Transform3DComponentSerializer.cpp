#include "ECSpch.h"
#include "Transform3DComponentSerializer.h"

namespace Coco::ECS
{
	void Transform3DComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		const Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();

		emitter << YAML::Key << "localPosition" << YAML::Value << transform._transform.LocalPosition;
		emitter << YAML::Key << "localRotation" << YAML::Value << transform._transform.LocalRotation;
		emitter << YAML::Key << "localScale" << YAML::Value << transform._transform.LocalScale;
		emitter << YAML::Key << "inheritParentTransform" << YAML::Value << transform._inheritParentTransform;
		emitter << YAML::Key << "transformReparentMode" << YAML::Value << static_cast<int>(transform._reparentMode);
	}

	bool Transform3DComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		Vector3 position = baseNode["localPosition"].as<Vector3>();
		Quaternion rotation = baseNode["localRotation"].as<Quaternion>();
		Vector3 scale = baseNode["localScale"].as<Vector3>();
		bool inherit = baseNode["inheritParentTransform"].as<bool>();

		Transform3DComponent& transform = entity.AddComponent<Transform3DComponent>(position, rotation, scale, inherit);
		transform._reparentMode = static_cast<TransformReparentMode>(baseNode["transformReparentMode"].as<int>());

		return true;
	}
}