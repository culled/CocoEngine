#include "ECSpch.h"
#include "MeshRendererComponent.h"

using namespace Coco::Rendering;

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent(const Entity& owner) :
		MeshRendererComponent(owner, nullptr, std::unordered_map<uint32, SharedRef<Material>>(), 1)
	{}

	MeshRendererComponent::MeshRendererComponent(
		const Entity& owner, 
		SharedRef<Rendering::Mesh> mesh, 
		const std::unordered_map<uint32, SharedRef<Material>>& materials, 
		uint64 visibilityGroups) :
		EntityComponent(owner),
		_visibilityGroups(visibilityGroups),
		_mesh(mesh),
		_materials(materials)
	{
		EnsureMaterialSlots();
	}

	void MeshRendererComponent::SetMesh(SharedRef<Mesh> mesh)
	{
		_mesh = mesh;

		EnsureMaterialSlots();
	}

	void MeshRendererComponent::SetMaterial(uint32 slotIndex, SharedRef<Material> material)
	{
		if (!_materials.contains(slotIndex))
			return;

		_materials[slotIndex] = material;
	}

	void MeshRendererComponent::SetMaterials(const std::unordered_map<uint32, SharedRef<Material>>& materials)
	{
		_materials = materials;

		// Cleanup extra slots
		EnsureMaterialSlots();
	}

	void MeshRendererComponent::SetVisibilityGroups(uint64 visibilityGroups)
	{
		_visibilityGroups = visibilityGroups;
	}

	void MeshRendererComponent::EnsureMaterialSlots()
	{
		if (!_mesh)
		{
			_materials.clear();
			return;
		}

		const auto& submesh = _mesh->GetSubmeshes();

		// Remove slots not in the submesh
		auto materialIt = _materials.begin();
		while (materialIt != _materials.end())
		{
			if (submesh.contains(materialIt->first))
			{
				materialIt++;
			}
			else
			{
				materialIt = _materials.erase(materialIt);
			}
		}
	
		// Add missing slots from the submesh
		for (const auto& it : submesh)
		{
			if (_materials.contains(it.first))
				continue;
	
			_materials[it.first] = nullptr;
		}
	}
}