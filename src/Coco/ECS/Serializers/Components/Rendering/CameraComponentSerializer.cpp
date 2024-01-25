#include "ECSpch.h"
#include "CameraComponentSerializer.h"

namespace Coco::ECS
{
	void CameraComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		const CameraComponent& camera = entity.GetComponent<CameraComponent>();

		emitter << YAML::Key << "priority" << YAML::Value << camera._priority;
		emitter << YAML::Key << "clearColor" << YAML::Value << camera._clearColor;
		emitter << YAML::Key << "msaaSamples" << YAML::Value << static_cast<int>(camera._sampleCount);
		emitter << YAML::Key << "projectionType" << YAML::Value << static_cast<int>(camera._projectionType);

		emitter << YAML::Key << "perspectiveFOV" << YAML::Value << camera._perspectiveFOV;
		emitter << YAML::Key << "perspectiveClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera._perspectiveNearClip << camera._perspectiveFarClip << YAML::EndSeq;

		emitter << YAML::Key << "orthoSize" << YAML::Value << camera._orthoSize;
		emitter << YAML::Key << "orthoClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera._orthoNearClip << camera._orthoFarClip << YAML::EndSeq;
	}

	bool CameraComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		CameraComponent& camera = entity.AddComponent<CameraComponent>();

		camera._priority = baseNode["priority"].as<int>();
		camera._clearColor = baseNode["clearColor"].as<Color>();
		camera._sampleCount = static_cast<Rendering::MSAASamples>(baseNode["msaaSamples"].as<int>());
		camera._projectionType = static_cast<CameraProjectionType>(baseNode["projectionType"].as<int>());

		camera._perspectiveFOV = baseNode["perspectiveFOV"].as<double>();

		const YAML::Node perspectiveClipNode = baseNode["perspectiveClipping"];
		camera._perspectiveNearClip = perspectiveClipNode[0].as<double>();
		camera._perspectiveFarClip = perspectiveClipNode[1].as<double>();

		camera._orthoSize = baseNode["orthoSize"].as<double>();

		const YAML::Node orthoClipNode = baseNode["orthoClipping"];
		camera._orthoNearClip = orthoClipNode[0].as<double>();
		camera._orthoFarClip = orthoClipNode[1].as<double>();

		return true;
	}
}