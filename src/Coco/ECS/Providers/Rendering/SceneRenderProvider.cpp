#include "ECSpch.h"
#include "SceneRenderProvider.h"

#include "../../Components/EntityInfoComponent.h"
#include "../../Components/Transform3DComponent.h"
#include "../../Components/Rendering/MeshRendererComponent.h"

#include "../../SceneView.h"

namespace Coco::ECS
{

	SceneRender3DProvider::SceneRender3DProvider(SharedRef<Scene> scene) :
		_scene(scene)
	{}

	void SceneRender3DProvider::GatherSceneData(RenderView& renderView)
	{
		Assert(_scene != nullptr)

		auto view = SceneView<MeshRendererComponent, Transform3DComponent, EntityInfoComponent>(_scene);
		const ViewFrustum& viewFrustum = renderView.GetViewFrustum();

		for (Entity& e : view)
		{
			const EntityInfoComponent& info = e.GetComponent<EntityInfoComponent>();
			const Transform3DComponent& transform = e.GetComponent<Transform3DComponent>();
			const MeshRendererComponent& renderer = e.GetComponent<MeshRendererComponent>();

			SharedRef<Mesh> mesh = renderer.GetMesh();

			if (!info.IsActiveInHierarchy() || !mesh)
				continue;

			Matrix4x4 globalTransform = transform.GetTransformMatrix(TransformSpace::Self, TransformSpace::Global);
			const auto& materials = renderer.GetMaterials();

			for (const auto& kvp : mesh->GetSubmeshes())
			{
				BoundingBox b = kvp.second.Bounds.Transformed(globalTransform);

				// Skip meshes outside our frustum
				if (!viewFrustum.IsInside(b))
					continue;

				auto it = materials.find(kvp.first);

				const MaterialDataProvider* material = nullptr;
				if (it != materials.end() && it->second)
					material = it->second.get();

				renderView.AddRenderObject(
					e.GetID(),
					*mesh,
					kvp.first,
					globalTransform,
					renderer.GetVisibilityGroups(),
					material,
					&b);
			}
		}
	}
}