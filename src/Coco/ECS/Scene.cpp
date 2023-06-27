#include "Scene.h"

#include "Components/MeshRendererComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"

namespace Coco::ECS
{
	Scene::Scene(SceneID id, string name, SceneID parentID) : _id(id), _name(name), _parentID(parentID)
	{}

	void Scene::GetSceneData(Ref<Rendering::RenderView> renderView)
	{
		ECSService* ecs = ECSService::Get();
	
		for (const auto& entityID : SceneView<TransformComponent, MeshRendererComponent>(*this))
		{
			MeshRendererComponent& renderer = ecs->GetComponent<MeshRendererComponent>(entityID);
			TransformComponent& transform = ecs->GetComponent<TransformComponent>(entityID);
	
			renderView->AddRenderObject(renderer.GetMesh(), renderer.GetMaterial(), transform.GetGlobalTransformMatrix());
		}
	}
}
