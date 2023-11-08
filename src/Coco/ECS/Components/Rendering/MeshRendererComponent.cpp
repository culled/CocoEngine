#include "ECSpch.h"
#include "MeshRendererComponent.h"

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent() :
		MeshRendererComponent(nullptr, std::unordered_map<uint32, SharedRef<MaterialDataProvider>>(), 1)
	{}

	MeshRendererComponent::MeshRendererComponent(SharedRef<Rendering::Mesh> mesh, const std::unordered_map<uint32, SharedRef<MaterialDataProvider>>& materials) :
		MeshRendererComponent(mesh, materials, 1)
	{}

	MeshRendererComponent::MeshRendererComponent(
		SharedRef<Rendering::Mesh> mesh, 
		const std::unordered_map<uint32, SharedRef<MaterialDataProvider>>& materials, 
		uint64 visibilityGroups) :
		VisibilityGroups(visibilityGroups),
		Mesh(mesh),
		Materials(materials)
	{}

	void MeshRendererComponent::EnsureMaterialSlots()
	{
		if (!Mesh)
		{
			Materials.clear();
			return;
		}

		for (const auto& it : Mesh->GetSubmeshes())
		{
			if (Materials.contains(it.first))
				continue;

			Materials[it.first] = nullptr;
		}
	}
}