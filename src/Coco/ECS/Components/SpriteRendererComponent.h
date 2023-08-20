#pragma once

#include "../EntityComponent.h"

#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>

namespace Coco::ECS
{
	/// @brief A component that renders a 2D texture with a material
	class COCOAPI SpriteRendererComponent : public EntityComponent
	{
	private:
		static Ref<Rendering::Mesh> _defaultSpriteMesh;
		Ref<Rendering::Material> _material;
		Ref<Rendering::Mesh> _customMesh;

	public:
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const EntityID& owner);
		SpriteRendererComponent(const EntityID& owner, Ref<Rendering::Material> material, Ref<Rendering::Mesh> customMesh = Ref<Rendering::Mesh>::Empty);

		virtual ~SpriteRendererComponent() = default;

		void SetMaterial(Ref<Rendering::Material> material) { _material = material; }
		Ref<Rendering::Material> GetMaterial() { return _material; }

		void SetCustomMesh(Ref<Rendering::Mesh> mesh) { _customMesh = mesh; }
		void ClearCustomMesh() { _customMesh = Ref<Rendering::Mesh>::Empty; }
		Ref<Rendering::Mesh> GetCustomMesh() { return _customMesh; }
		Ref<Rendering::Mesh> GetMesh();
	};
}