#include "ECSpch.h"
#include "MeshRendererComponentSerializer.h"

#include <Coco/Core/Engine.h>

using namespace Coco::Rendering;

namespace Coco::ECS
{
	void MeshRendererComponentSerializer::Serialize(YAML::Emitter& emitter, const Entity& entity)
	{
		const MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		emitter << YAML::Key << "visibility" << YAML::Value << renderer._visibilityGroups;
		emitter << YAML::Key << "mesh" << YAML::Value;

		if (renderer._mesh)
		{
			emitter << *renderer._mesh;
		}
		else
		{
			emitter << "";
		}

		emitter << YAML::Key << "materials" << YAML::Value << YAML::BeginMap;

		for (const auto& it : renderer._materials)
		{
			SharedRef<Material> material = it.second;

			emitter << YAML::Key << it.first << YAML::Value;

			if (material)
			{
				emitter << *material;
			}
			else
			{
				emitter << "";
			}
		}

		emitter << YAML::EndMap;
	}

	bool MeshRendererComponentSerializer::Deserialize(const YAML::Node& baseNode, Entity& entity)
	{
		uint64 visibilityGroups = baseNode["visibility"].as<uint64>();

		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(LoadResourceFromYAML(baseNode["mesh"]));

		std::unordered_map<uint32, SharedRef<Material>> materials;

		const YAML::Node materialsNode = baseNode["materials"];
		for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); it++)
		{
			uint32 id = it->first.as<uint32>();
			string path = it->second.as<string>();

			SharedRef<Material>& mat = materials[id];
			mat = std::dynamic_pointer_cast<Material>(LoadResourceFromYAML(it->second));
		}

		entity.AddComponent<MeshRendererComponent>(mesh, materials, visibilityGroups);

		return true;
	}
}