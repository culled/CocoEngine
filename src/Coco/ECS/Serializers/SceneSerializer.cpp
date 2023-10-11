#include "ECSpch.h"
#include "SceneSerializer.h"

#include "../Scene.h"

#include "../Components/EntityInfoComponent.h"
#include "../Components/Transform3DComponent.h"
#include "../Components/Rendering/CameraComponent.h"
#include "../Components/Rendering/MeshRendererComponent.h"
#include <Coco/Core/Engine.h>

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::ECS
{
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

		// TODO: make component registry and call serialization functions for each component
		{
			const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();
			emitter << YAML::Key << typeid(EntityInfoComponent).name() << YAML::Value << YAML::BeginMap;

			emitter << YAML::Key << "name" << YAML::Value << info.Name;
			emitter << YAML::Key << "id" << YAML::Value << info.ID;
			emitter << YAML::Key << "isActive" << YAML::Value << info.IsActive;

			if (info.Parent.has_value())
			{
				const EntityInfoComponent& parentInfo = info.Parent->GetComponent<EntityInfoComponent>();
				emitter << YAML::Key << "parent" << YAML::Value << parentInfo.ID;
			}

			emitter << YAML::EndMap;
		}

		if (entity.HasComponent<Transform3DComponent>())
		{
			const Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();
			emitter << YAML::Key << typeid(Transform3DComponent).name() << YAML::Value << YAML::BeginMap;

			emitter << YAML::Key << "localPosition" << YAML::Value << transform.Transform.LocalPosition;
			emitter << YAML::Key << "localRotation" << YAML::Value << transform.Transform.LocalRotation;
			emitter << YAML::Key << "localScale" << YAML::Value << transform.Transform.LocalScale;
			emitter << YAML::Key << "inheritParentTransform" << YAML::Value << transform.InheritParentTransform;

			emitter << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			const CameraComponent& camera = entity.GetComponent<CameraComponent>();
			emitter << YAML::Key << typeid(CameraComponent).name() << YAML::Value << YAML::BeginMap;

			emitter << YAML::Key << "priority" << YAML::Value << camera.Priority;
			emitter << YAML::Key << "clearColor" << YAML::Value << camera.ClearColor;
			emitter << YAML::Key << "msaaSamples" << YAML::Value << static_cast<int>(camera.SampleCount);
			emitter << YAML::Key << "projectionType" << YAML::Value << static_cast<int>(camera.ProjectionType);

			emitter << YAML::Key << "perspectiveFOV" << YAML::Value << camera.PerspectiveFOV;
			emitter << YAML::Key << "perspectiveClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera.PerspectiveNearClip << camera.PerspectiveFarClip << YAML::EndSeq;

			emitter << YAML::Key << "orthoSize" << YAML::Value << camera.OrthoSize;
			emitter << YAML::Key << "orthoClipping" << YAML::Value << YAML::Flow << YAML::BeginSeq << camera.OrthoNearClip << camera.OrthoFarClip << YAML::EndSeq;

			emitter << YAML::EndMap;
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			const MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();
			emitter << YAML::Key << typeid(MeshRendererComponent).name() << YAML::Value << YAML::BeginMap;

			emitter << YAML::Key << "mesh" << YAML::Value << (renderer.Mesh ? renderer.Mesh->GetContentPath() : "");
			emitter << YAML::Key << "materials" << YAML::Value << YAML::BeginMap;

			for (const auto& it : renderer.Materials)
			{
				SharedRef<Resource> resource = nullptr;

				if (it.second)
					resource = std::dynamic_pointer_cast<Resource>(it.second);

				emitter << YAML::Key << it.first << YAML::Value << (resource ? resource->GetContentPath() : "");
			}

			emitter << YAML::EndMap;
			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEntity(SharedRef<Scene>& scene, const YAML::Node& entityNode)
	{
		const YAML::Node entityInfo = entityNode[typeid(EntityInfoComponent).name()];
		string name = entityInfo["name"].as<string>();
		EntityID id = entityInfo["id"].as<uint64>();

		Entity e = scene->CreateEntity(name, id);
		EntityInfoComponent& info = e.GetComponent<EntityInfoComponent>();
		info.IsActive = entityInfo["isActive"].as<bool>();

		if (entityInfo["parent"])
		{
			// TODO: parenting
		}

		if (entityNode[typeid(Transform3DComponent).name()])
		{
			const YAML::Node transformNode = entityNode[typeid(Transform3DComponent).name()];

			Vector3 position = transformNode["localPosition"].as<Vector3>();
			Quaternion rotation = transformNode["localRotation"].as<Quaternion>();
			Vector3 scale = transformNode["localScale"].as<Vector3>();
			bool inherit = transformNode["inheritParentTransform"].as<bool>();

			e.AddComponent<Transform3DComponent>(position, rotation, scale, inherit);
		}

		if (entityNode[typeid(CameraComponent).name()])
		{
			const YAML::Node cameraNode = entityNode[typeid(CameraComponent).name()];
			CameraComponent& camera = e.AddComponent<CameraComponent>();

			camera.Priority = cameraNode["priority"].as<int>();
			camera.ClearColor = cameraNode["clearColor"].as<Color>();
			camera.SampleCount = static_cast<Rendering::MSAASamples>(cameraNode["msaaSamples"].as<int>());
			camera.ProjectionType = static_cast<CameraProjectionType>(cameraNode["projectionType"].as<int>());

			camera.PerspectiveFOV = cameraNode["perspectiveFOV"].as<double>();

			const YAML::Node perspectiveClipNode = cameraNode["perspectiveClipping"];
			camera.PerspectiveNearClip = perspectiveClipNode[0].as<double>();
			camera.PerspectiveFarClip = perspectiveClipNode[1].as<double>();

			camera.OrthoSize = cameraNode["orthoSize"].as<double>();
			const YAML::Node orthoClipNode = cameraNode["orthoClipping"];
			camera.OrthoNearClip = orthoClipNode[0].as<double>();
			camera.OrthoFarClip = orthoClipNode[1].as<double>();
		}

		if (entityNode[typeid(MeshRendererComponent).name()])
		{
			const YAML::Node rendererNode = entityNode[typeid(MeshRendererComponent).name()];

			ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

			SharedRef<Mesh> mesh = resources.GetOrLoad<Mesh>(rendererNode["mesh"].as<string>());

			std::unordered_map<uint32, SharedRef<MaterialDataProvider>> materials;

			const YAML::Node materialsNode = rendererNode["materials"];
			for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); it++)
			{
				uint32 id = it->first.as<uint32>();
				string path = it->second.as<string>();

				SharedRef<MaterialDataProvider>& mat = materials[id];

				if (!path.empty())
				{
					SharedRef<Resource> materialResource;
					if (resources.GetOrLoad(path, materialResource))
						mat = std::dynamic_pointer_cast<MaterialDataProvider>(materialResource);
				}
			}

			e.AddComponent<MeshRendererComponent>(mesh, materials);
		}
	}
}