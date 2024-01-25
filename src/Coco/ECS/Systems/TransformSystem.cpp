#include "ECSpch.h"
#include "TransformSystem.h"

#include "../ECSService.h"
#include "../Entity.h"
#include "../Scene.h"
#include "../Components/Transform3DComponent.h"

namespace Coco::ECS
{
	EventHandler<Entity&> TransformSystem::_entityParentChangedHandler([](Entity& e) { TransformSystem::RecalculateEntityTransforms(e); return false; });

	TransformSystem::TransformSystem(Scene& scene) :
		SceneSystem(scene)
	{
		if(!_entityParentChangedHandler.IsConnected())
			_entityParentChangedHandler.Connect(ECSService::Get()->OnEntityParentChanged);
	}

	TransformSystem::~TransformSystem()
	{}

	void TransformSystem::EntitiesAdded(std::span<Entity> rootEntities)
	{
		for (Entity& e : rootEntities)
			RecalculateEntityTransforms(e);
	}

	void TransformSystem::RecalculateEntityTransforms(Entity& entity)
	{
		Transform3DComponent* transform = nullptr;

		if (!entity.TryGetComponent<Transform3DComponent>(transform))
			return;

		if (transform->_reparentMode == TransformReparentMode::KeepWorldTransform)
		{
			Vector3 p, s;
			Quaternion r;
			transform->Decompose(TransformSpace::Global, p, r, s);

			const Transform3D* parent = nullptr;
			if (transform->TryGetParentTransform(parent))
			{
				parent->Transform(TransformSpace::Global, TransformSpace::Self, p, r, s);
			}

			transform->SetPosition(p, TransformSpace::Parent);
			transform->SetRotation(r, TransformSpace::Parent);
			transform->SetScale(s, TransformSpace::Parent);
		}

		transform->Recalculate();
	}
}