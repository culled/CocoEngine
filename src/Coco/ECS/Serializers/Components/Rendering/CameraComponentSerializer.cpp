#include "ECSpch.h"
#include "CameraComponentSerializer.h"

#include "../../../Components/Rendering/CameraComponent.h"

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::ECS
{
	void CameraComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const CameraComponent& camera = entity.GetComponent<CameraComponent>();

		emitter << YAML::Key << "priority" << YAML::Value << camera.Priority;
		emitter << YAML::Key << "clearColor" << YAML::Value << camera.ClearColor;
		emitter << YAML::Key << "msaaSamples" << YAML::Value << static_cast<int>(camera.SampleCount);
		emitter << YAML::Key << "projectionType" << YAML::Value << static_cast<int>(camera.ProjectionType);

		emitter << YAML::Key << "perspectiveFOV" << YAML::Value << camera.PerspectiveFOV;
		emitter << YAML::Key << "perspectiveClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera.PerspectiveNearClip << camera.PerspectiveFarClip << YAML::EndSeq;

		emitter << YAML::Key << "orthoSize" << YAML::Value << camera.OrthoSize;
		emitter << YAML::Key << "orthoClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera.OrthoNearClip << camera.OrthoFarClip << YAML::EndSeq;
	}

	void CameraComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		CameraComponent& camera = entity.AddComponent<CameraComponent>();

		camera.Priority = baseNode["priority"].as<int>();
		camera.ClearColor = baseNode["clearColor"].as<Color>();
		camera.SampleCount = static_cast<Rendering::MSAASamples>(baseNode["msaaSamples"].as<int>());
		camera.ProjectionType = static_cast<CameraProjectionType>(baseNode["projectionType"].as<int>());

		camera.PerspectiveFOV = baseNode["perspectiveFOV"].as<double>();

		const YAML::Node perspectiveClipNode = baseNode["perspectiveClipping"];
		camera.PerspectiveNearClip = perspectiveClipNode[0].as<double>();
		camera.PerspectiveFarClip = perspectiveClipNode[1].as<double>();

		camera.OrthoSize = baseNode["orthoSize"].as<double>();

		const YAML::Node orthoClipNode = baseNode["orthoClipping"];
		camera.OrthoNearClip = orthoClipNode[0].as<double>();
		camera.OrthoFarClip = orthoClipNode[1].as<double>();
	}
}