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
		List<Ref<Rendering::IMaterial>> _materials;
		Ref<Rendering::Mesh> _mesh;

	public:
		MeshRendererComponent() = default;
		MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, Ref<Rendering::IMaterial> material);
		MeshRendererComponent(const EntityID& ownerID, Ref<Rendering::Mesh> mesh, const List<Ref<Rendering::IMaterial>>& materials);

		/// @brief Sets the material for a slot that the mesh will be rendered with
		/// @param material The material
		/// @param slot The slot for the material. This should correspond with the submesh index of this renderer's mesh
		void SetMaterial(Ref<Rendering::IMaterial> material, uint slot);

		/// @brief Gets the materials that the mesh will be rendered with
		/// @return The materials
		List<Ref<Rendering::IMaterial>> GetMaterials() const { return _materials; }

		/// @brief Sets the mesh that will be rendered
		/// @param mesh The mesh
		void SetMesh(Ref<Rendering::Mesh> mesh) { _mesh = mesh; }

		/// @brief Gets the mesh that will be rendered
		/// @return The mesh
		Ref<Rendering::Mesh> GetMesh() { return _mesh; }
	};
}