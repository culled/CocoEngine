#include "Scene.h"

#include "Components/MeshRendererComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RectTransformComponent.h"
#include "Components/SpriteRendererComponent.h"
#include "Components/ScriptComponent.h"
#include "Entity.h"
#include "SceneView.h"

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
	
			renderView->AddRenderObject(renderer.GetMesh(), renderer.GetMaterials(), transform.GetGlobalTransformMatrix());
		}
		
		for (const auto& entityID : SceneView<RectTransformComponent, SpriteRendererComponent>(*this))
		{
			SpriteRendererComponent& renderer = ecs->GetComponent<SpriteRendererComponent>(entityID);
			RectTransformComponent& transform = ecs->GetComponent<RectTransformComponent>(entityID);

			renderView->AddRenderObject(renderer.GetMesh(), { renderer.GetMaterial() }, transform.GetGlobalTransformMatrix(true));
		}
	}

	void Scene::Tick(double deltaTime)
	{
		ECSService* ecs = ECSService::Get();

		for (const auto& entityID : SceneView<ScriptComponent>(*this))
		{
			ScriptComponent& script = ecs->GetComponent<ScriptComponent>(entityID);
			script.Tick(deltaTime);
		}
	}
}
