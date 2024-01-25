#include "ECSpch.h"
#include "SceneSerializer.h"

#include "../Scene.h"

#include "../Components/EntityInfoComponent.h"
#include "ComponentSerializerFactory.h"

#include <Coco/Core/Engine.h>

#include <yaml-cpp/yaml.h>
#include <Coco/Core/Types/YAMLConverters.h>

namespace Coco::ECS
{
	SceneSerializer::SceneSerializer()
	{}

	SceneSerializer::~SceneSerializer()
	{}

	bool SceneSerializer::SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource)
	{
		SharedRef<Scene> scene = std::dynamic_pointer_cast<Scene>(resource);

		if (!scene)
		{
			CocoError("Resource was not a scene resource")
			return false;
		}

		emitter << YAML::Key << "entities" << YAML::Value << YAML::BeginMap;

		std::vector<Entity> sceneRootEntities = scene->GetEntities(true);

		for (const Entity& entity : sceneRootEntities)
			SerializeEntity(emitter, entity);

		emitter << YAML::EndMap;

		return true;
	}

	SharedRef<Resource> SceneSerializer::CreateResource(const ResourceID& id)
	{
		return CreateSharedRef<Scene>(id);
	}

	bool SceneSerializer::DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource)
	{
		SharedRef<Scene> scene = std::dynamic_pointer_cast<Scene>(resource);

		if (!scene)
		{
			CocoError("Resource was not a scene resource")
			return false;
		}

		scene->DestroyAllEntities(false);

		std::vector<Entity> rootEntities;
		YAML::Node entitiesNode = baseNode["entities"];
		for (YAML::const_iterator it = entitiesNode.begin(); it != entitiesNode.end(); ++it)
		{
			rootEntities.push_back(DeserializeEntity(scene, it->first.as<string>(), it->second, Entity::Null));
		}

		scene->EntitiesAdded(rootEntities);

		return true;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, const Entity& entity)
	{
		const EntityInfoComponent& entityInfo = entity.GetComponent<EntityInfoComponent>();

		emitter << YAML::Key << entityInfo.GetName() << YAML::Value << YAML::BeginMap;

		emitter << YAML::Key << "components" << YAML::Value << YAML::BeginMap;
		for (auto& serializerKVP : ComponentSerializerFactory::GetMap())
		{
			auto serializer = ComponentSerializerFactory::Create(serializerKVP.first);

			if (!serializer->ShouldUseForEntity(entity))
				continue;

			emitter << YAML::Key << serializerKVP.first << YAML::BeginMap;
			serializer->Serialize(emitter, entity);
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndMap;

		emitter << YAML::Key << "childEntities" << YAML::Value << YAML::BeginMap;
		for (const Entity& child : entity.GetChildren())
		{
			SerializeEntity(emitter, child);
		}
		emitter << YAML::EndMap;

		emitter << YAML::EndMap;
	}

	Entity SceneSerializer::DeserializeEntity(SharedRef<Scene>& scene, const string& entityName, const YAML::Node& entityNode, const Entity& parent)
	{
		Entity entity = scene->CreateEntity(entityName, parent);

		YAML::Node childEntitiesNode = entityNode["childEntities"];
		for (YAML::const_iterator it = childEntitiesNode.begin(); it != childEntitiesNode.end(); ++it)
		{
			DeserializeEntity(scene, it->first.as<string>(), it->second, entity);
		}

		YAML::Node componentsNode = entityNode["components"];
		for (YAML::const_iterator it = componentsNode.begin(); it != componentsNode.end(); ++it)
		{
			string componentTypename = it->first.as<string>();
			if (!ComponentSerializerFactory::Has(componentTypename))
			{
				CocoError("Unknown component type \"{}\"", componentTypename)
				continue;
			}

			auto serializer = ComponentSerializerFactory::Create(componentTypename);
			serializer->Deserialize(it->second, entity);
		}

		return entity;
	}
}