#include "ECSpch.h"
#include "SceneRenderProvider.h"

#include "../EntityInfoSystem.h"
#include "../../Components/Transform3DComponent.h"
#include "../../Components/Rendering/MeshRendererComponent.h"
#include "../TransformSystem.h"

#include "../../SceneView.h"

namespace Coco::ECS
{

	SceneRender3DProvider::SceneRender3DProvider(SharedRef<Scene> scene) :
		_scene(scene)
	{}

	void SceneRender3DProvider::GatherSceneData(RenderView& renderView)
	{
		Assert(_scene != nullptr)

		auto view = SceneView<Transform3DComponent, MeshRendererComponent>(_scene);
		const ViewFrustum& viewFrustum = renderView.GetViewFrustum();

		for (Entity& e : view)
		{
			const Transform3DComponent& transform = e.GetComponent<Transform3DComponent>();
			const MeshRendererComponent& renderer = e.GetComponent<MeshRendererComponent>();

			if (!EntityInfoSystem::IsEntityVisible(e) || !renderer.Mesh)
				continue;

			TransformSystem::UpdateTransform3D(e);

			for (const auto& kvp : renderer.Mesh->GetSubmeshes())
			{
				BoundingBox b = kvp.second.Bounds.Transformed(transform.Transform.GlobalTransform);

				// Skip meshes outside our frustum
				if (!viewFrustum.IsInside(b))
					continue;

				auto it = renderer.Materials.find(kvp.first);

				const MaterialDataProvider* material = nullptr;
				if (it != renderer.Materials.end() && it->second)
					material = it->second.get();

				renderView.AddRenderObject(
					e.GetID(),
					*renderer.Mesh,
					kvp.first,
					transform.Transform.GlobalTransform,
					renderer.VisibilityGroups,
					material,
					&b);
			}
		}
	}
}