#include "ECSpch.h"
#include "SceneRenderProvider.h"

#include "../../Components/EntityInfoComponent.h"
#include "../../Components/Transform3DComponent.h"
#include "../../Components/Rendering/MeshRendererComponent.h"

namespace Coco::ECS
{

	SceneRender3DProvider::SceneRender3DProvider(SharedRef<Scene> scene) :
		_scene(scene)
	{}

	void SceneRender3DProvider::GatherSceneData(RenderView& renderView)
	{
		Assert(_scene != nullptr)

		auto view = _scene->_registry.view<EntityInfoComponent, Transform3DComponent, MeshRendererComponent>();

		for (const auto& e : view)
		{
			const EntityInfoComponent& info = _scene->_registry.get<const EntityInfoComponent>(e);
			const Transform3DComponent& transform = _scene->_registry.get<const Transform3DComponent>(e);
			const MeshRendererComponent& renderer = _scene->_registry.get<const MeshRendererComponent>(e);

			if (!info.IsActive || !renderer.Mesh)
				continue;

			for (const auto& kvp : renderer.Mesh->GetSubmeshes())
			{
				if (!renderer.Materials.contains(kvp.first))
					continue;

				renderView.AddRenderObject(
					*renderer.Mesh,
					kvp.first,
					transform.Transform.GlobalTransform,
					*renderer.Materials.at(kvp.first));
			}
		}
	}
}