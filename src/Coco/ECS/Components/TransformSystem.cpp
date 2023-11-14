#include "ECSpch.h"
#include "TransformSystem.h"

#include "../Entity.h"
#include "../Scene.h"
#include "../SceneView.h"
#include "Transform3DComponent.h"

namespace Coco::ECS
{
	const int TransformSystem::sPriority = -1000;

	TransformSystem::TransformSystem(SharedRef<Scene> scene) :
		SceneSystem(scene),
		_entityParentChangedHandler([](Entity& e) { RecalculateEntityTransforms(e, true); return false; })
	{
		_entityParentChangedHandler.Connect(scene->OnEntityParentChanged);
	}

	TransformSystem::~TransformSystem()
	{
		_entityParentChangedHandler.DisconnectAll();
	}

	void TransformSystem::Tick()
	{}

	void TransformSystem::RecalculateEntityTransforms(const Entity& entity, bool preserveGlobalSpace)
	{
		Transform3DComponent* transform = nullptr;

		if (!entity.TryGetComponent<Transform3DComponent>(transform))
			return;

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

		transform->Recalculate();
	}
}