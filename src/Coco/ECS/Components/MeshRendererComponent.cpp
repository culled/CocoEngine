#include "MeshRendererComponent.h"

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::IMaterial> material) : EntityComponent(ownerID),
		_mesh(mesh), _material(material)
	{}
}
