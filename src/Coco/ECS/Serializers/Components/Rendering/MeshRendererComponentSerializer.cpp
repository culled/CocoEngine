#include "ECSpch.h"
#include "MeshRendererComponentSerializer.h"

#include "../../../Components/Rendering/MeshRendererComponent.h"
#include <Coco/Core/Engine.h>

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::ECS
{
	void MeshRendererComponentSerializer::SerializeImpl(YAML::Emitter& emitter, const Entity& entity)
	{
		const MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		emitter << YAML::Key << "visibility" << YAML::Value << renderer._visibilityGroups;
		emitter << YAML::Key << "mesh" << YAML::Value << (renderer._mesh ? renderer._mesh->GetContentPath() : "");
		emitter << YAML::Key << "materials" << YAML::Value << YAML::BeginMap;

		for (const auto& it : renderer._materials)
		{
			SharedRef<Resource> resource = nullptr;

			if (it.second)
				resource = std::dynamic_pointer_cast<Resource>(it.second);

			emitter << YAML::Key << it.first << YAML::Value << (resource ? resource->GetContentPath() : "");
		}

		emitter << YAML::EndMap;
	}

	void MeshRendererComponentSerializer::DeserializeImpl(const YAML::Node& baseNode, Entity& entity)
	{
		uint64 visibilityGroups = baseNode["visibility"].as<uint64>();

		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		SharedRef<Mesh> mesh = resources.GetOrLoad<Mesh>(baseNode["mesh"].as<string>());

		std::unordered_map<uint32, SharedRef<MaterialDataProvider>> materials;

		const YAML::Node materialsNode = baseNode["materials"];
		for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); it++)
		{
			uint32 id = it->first.as<uint32>();
			string path = it->second.as<string>();

			SharedRef<MaterialDataProvider>& mat = materials[id];

			if (!path.empty())
			{
				SharedRef<Resource> materialResource;
				if (resources.GetOrLoad(path, false, materialResource))
					mat = std::dynamic_pointer_cast<MaterialDataProvider>(materialResource);
			}
		}

		entity.AddComponent<MeshRendererComponent>(mesh, materials, visibilityGroups);
	}
}