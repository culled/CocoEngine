#include "ECSpch.h"
#include "TransformSystem.h"

#include "../Entity.h"
#include "../Scene.h"
#include "../Components/Transform3DComponent.h"
#include "../SceneView.h"

namespace Coco::ECS
{
	const int TransformSystem::sPriority = -1000;

	void TransformSystem::Execute(SharedRef<Scene> scene)
	{
		Update3DTransforms(scene);
	}

	void TransformSystem::UpdateTransform3D(Entity& entity, bool forceUpdate)
	{
		Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();

		if (!forceUpdate && !transform.IsDirty)
			return;

		Transform3DComponent* parentTransform = nullptr;

		if (transform.InheritParentTransform && entity.HasParent())
		{
			Entity parent = entity.GetParent();
			UpdateTransform3D(parent);
			parentTransform = &parent.GetComponent<Transform3DComponent>();
		}

		transform.Transform.Recalculate(&parentTransform->Transform);
		transform.IsDirty = false;
	}

	void TransformSystem::MarkTransform3DDirty(Entity& entity)
	{
		Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();
		if (transform.IsDirty)
			return;

		transform.IsDirty = true;

		// Mark all children as dirty too
		std::vector<Entity> children = entity.GetChildren();
		for (Entity& child : children)
			MarkTransform3DDirty(child);
	}

	bool TransformSystem::TryGetParentTransform3D(Entity& entity, Transform3DComponent*& outTransform)
	{
		if (!entity.HasParent())
			return false;

		return entity.GetParent().TryGetComponent<Transform3DComponent>(outTransform);
	}

	void TransformSystem::Update3DTransforms(SharedRef<Scene>& scene)
	{
		SceneView<Transform3DComponent> view(scene);

		for (Entity& entity : view)
		{
			UpdateTransform3D(entity);
		}
	}
}