#include "MeshRendererComponent.h"

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::IMaterial> material) :
		EntityComponent(ownerID),
		_mesh(mesh), _materials(1)
	{
		SetMaterial(material, 0);
	}

	MeshRendererComponent::MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, const List<Ref<Rendering::IMaterial>>& materials) :
		EntityComponent(ownerID),
		_mesh(mesh), _materials(materials)
	{}

	void MeshRendererComponent::SetMaterial(Ref<Rendering::IMaterial> material, uint slot)
	{
		if (_materials.Count() <= slot)
			_materials.Resize(slot + 1);

		_materials[slot] = material;
	}
}
