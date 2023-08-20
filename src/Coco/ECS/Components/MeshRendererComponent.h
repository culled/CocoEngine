#pragma once

#include "../EntityComponent.h"

#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>

namespace Coco::ECS
{
	/// @brief A component that renders a 3D mesh with a material
	class COCOAPI MeshRendererComponent : public EntityComponent
	{
	private:
		Ref<Rendering::Material> _material;
		Ref<Rendering::Mesh> _mesh;

	public:
		MeshRendererComponent() = default;
		MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::Material> material);

		/// @brief Sets the material that the mesh will be rendered with
		/// @param material The material
		void SetMaterial(Ref<Rendering::Material> material) { _material = material; }

		/// @brief Gets the material that the mesh will be rendered with
		/// @return The material
		Ref<Rendering::Material> GetMaterial() { return _material; }

		/// @brief Sets the mesh that will be rendered
		/// @param mesh The mesh
		void SetMesh(Ref<Rendering::Mesh> mesh) { _mesh = mesh; }

		/// @brief Gets the mesh that will be rendered
		/// @return The mesh
		Ref<Rendering::Mesh> GetMesh() { return _mesh; }
	};
}