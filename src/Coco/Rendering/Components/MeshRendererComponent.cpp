#include "MeshRendererComponent.h"

namespace Coco::Rendering
{
	MeshRendererComponent::MeshRendererComponent(Coco::SceneEntity* entity, Ref<Mesh> mesh, Ref<Material> material) : EntityComponent(entity),
		_mesh(mesh), _material(material)
	{}
}
