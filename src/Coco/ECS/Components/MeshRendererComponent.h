#pragma once

#include "../EntityComponent.h"

#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>

namespace Coco::ECS
{
	class COCOAPI MeshRendererComponent : public EntityComponent
	{
	private:
		Ref<Rendering::Material> _material;
		Ref<Rendering::Mesh> _mesh;

	public:
		MeshRendererComponent() = default;
		MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::Material> material);

		Ref<Rendering::Material> GetMaterial() { return _material; }
		Ref<Rendering::Mesh> GetMesh() { return _mesh; }
	};
}