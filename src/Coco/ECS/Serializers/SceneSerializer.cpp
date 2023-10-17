#include "ECSpch.h"
#include "SceneSerializer.h"

#include "../Scene.h"

#include "Components/EntityInfoComponentSerializer.h"
#include "../Components/EntityInfoComponent.h"

#include "Components/Transform3DComponentSerializer.h"
#include "../Components/Transform3DComponent.h"

#include "Components/Rendering/CameraComponentSerializer.h"
#include "../Components/Rendering/CameraComponent.h"

#include "Components/Rendering/MeshRendererComponentSerializer.h"
#include "../Components/Rendering/MeshRendererComponent.h"

#include <Coco/Core/Engine.h>

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::ECS
{
	ComponentSerializerInstance::ComponentSerializerInstance(
		TestFunctionType testFunc,
		UniqueRef<ComponentSerializer>&& serializer) :
		TestFunc(testFunc),
		Serializer(std::move(serializer))
	{}

	std::vector<ComponentSerializerInstance> SceneSerializer::_sComponentSerializers;

	SceneSerializer::SceneSerializer()
	{
		_sComponentSerializers = std::vector<ComponentSerializerInstance>();
		AddDefaultSerializers();
	}

	SceneSerializer::~SceneSerializer()
	{
		_sComponentSerializers.clear();
	}

	bool SceneSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".cscene";
	}

	bool SceneSerializer::SupportsResourceType(const std::type_index& type) const
	{
		return type == typeid(Scene);
	}

	const std::type_index SceneSerializer::GetResourceTypeForExtension(const string& extension) const
	{
		return typeid(Scene);
	}

	string SceneSerializer::Serialize(SharedRef<Resource> resource)
	{
		SharedRef<Scene> scene = std::dynamic_pointer_cast<Scene>(resource);

		Assert(scene)

		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "name" << YAML::Value << scene->GetName();
		out << YAML::Key << "entities" << YAML::Value << YAML::BeginSeq;

		scene->EachEntity([&](Entity& e) 
			{
				SerializeEntity(out, e);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap << YAML::Comment("Fix");

		return out.c_str();
	}

	SharedRef<Resource> SceneSerializer::Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);

		string name = baseNode["name"].as<string>();

		SharedRef<Scene> scene = CreateSharedRef<Scene>(resourceID, name);

		YAML::Node entitiesNode = baseNode["entities"];
		for (YAML::const_iterator it = entitiesNode.begin(); it != entitiesNode.end(); it++)
		{
			DeserializeEntity(scene, *it);
		}

		return scene;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, const Entity& entity)
	{
		emitter << YAML::BeginMap;

		for (auto& serializer : _sComponentSerializers)
		{
			if (!serializer.TestFunc(entity))
				continue;

			serializer.Serializer->Serialize(emitter, entity);
		}

		emitter << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEntity(SharedRef<Scene>& scene, const YAML::Node& entityNode)
	{
		Entity entity = scene->CreateEntity("Entity", 0);

		for (auto& serializer : _sComponentSerializers)
		{
			serializer.Serializer->Deserialize(entityNode, entity);
		}
	}

	void SceneSerializer::AddDefaultSerializers()
	{
		RegisterComponentSerializer<EntityInfoComponent, EntityInfoComponentSerializer>();
		RegisterComponentSerializer<Transform3DComponent, Transform3DComponentSerializer>();
		RegisterComponentSerializer<CameraComponent, CameraComponentSerializer>();
		RegisterComponentSerializer<MeshRendererComponent, MeshRendererComponentSerializer>();
	}
}