#include "MeshRendererComponent.h"

namespace Coco::ECS
{
	MeshRendererComponent::MeshRendererComponent(EntityID ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::Material> material) : EntityComponent(ownerID),
		_mesh(mesh), _material(material)
	{}
}
