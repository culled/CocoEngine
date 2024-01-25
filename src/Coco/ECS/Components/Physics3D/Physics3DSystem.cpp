#include "ECSpch.h"
#include "Physics3DSystem.h"
#include "../Transform3DComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "../../SceneView.h"

#include <Coco/Physics3D/Physics3DService.h>

using namespace Coco::Physics3D;

namespace Coco::ECS
{
	const int Physics3DSystem::sSetupPriority = -1000;
	const int Physics3DSystem::sLateTickPriority = 100;
	const int Physics3DSystem::sEarlyTickPriority = -100;

	Physics3DSystem::Physics3DSystem(SharedRef<Scene> scene) :
		SceneSystem(scene)
	{
		RegisterTickListener(this, &Physics3DSystem::HandleEarlyTick, sEarlyTickPriority);
		RegisterTickListener(this, &Physics3DSystem::HandleLateTick, sLateTickPriority);
	}

	Physics3DSystem::~Physics3DSystem()
	{}

	void Physics3DSystem::SimulationStarted()
	{
		Physics3DService* physics = Physics3DService::Get();

		if (!physics)
			return;

		// Setup the physics world for the scene
		_physicsWorld3D = physics->GetPlatform().CreateWorld();

		SceneView<Rigidbody3DComponent, Transform3DComponent> view(_scene.lock());
		
		// Setup all rigidbodies
		for (const auto& e : view)
		{
			Rigidbody3DComponent& body = e.GetComponent<Rigidbody3DComponent>();
			const Transform3DComponent& transform = e.GetComponent<Transform3DComponent>();

			CreateRigidbody(body, transform);
			_physicsWorld3D->AddRigidbody(body._body);
		}
	}

	void Physics3DSystem::SimulationEnded()
	{
		Physics3DService* physics = Physics3DService::Get();

		if (!physics)
			return;

		Physics3DPlatform& platform = physics->GetPlatform();

		SceneView<Rigidbody3DComponent> view(_scene.lock());

		// Destroy all rigidbodies
		for (const auto& e : view)
		{
			Rigidbody3DComponent& body = e.GetComponent<Rigidbody3DComponent>();

			_physicsWorld3D->RemoveRigidbody(body._body);
			platform.TryDestroyRigidbody(body._body);
		}

		platform.TryDestroyWorld(_physicsWorld3D);
	}

	std::vector<SharedRef<CollisionShape3D>> Physics3DSystem::GatherCollisionShapes(const Entity& entity)
	{
		std::vector<SharedRef<CollisionShape3D>> shapes;

		// TODO: make this work with multiple shapes

		if (entity.HasComponent<BoxColliderComponent>())
		{
			const auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
			shapes.push_back(boxCollider.GetCollisionShape());
		}
		else if (entity.HasComponent<SphereColliderComponent>())
		{
			const auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
			shapes.push_back(sphereCollider.GetCollisionShape());
		}

		return shapes;
	}

	void Physics3DSystem::HandleEarlyTick(const TickInfo& tickInfo)
	{
		SceneView<Rigidbody3DComponent, Transform3DComponent> view(_scene.lock());

		for (const auto& e : view)
		{
			Rigidbody3DComponent& body = e.GetComponent<Rigidbody3DComponent>();
			Transform3DComponent& transform = e.GetComponent<Transform3DComponent>();

			Vector3 p;
			Quaternion r;

			body.GetWorldTransform(p, r);
			transform.SetPositionAndRotation(p, r, TransformSpace::Global);
		}
	}

	void Physics3DSystem::HandleLateTick(const TickInfo& tickInfo)
	{
		_physicsWorld3D->Simulate(tickInfo.DeltaTime);
	}

	void Physics3DSystem::CreateRigidbody(Rigidbody3DComponent& body, const Transform3DComponent& transform)
	{
		const Entity& entity = body.GetOwner();

		std::vector<SharedRef<CollisionShape3D>> collision = GatherCollisionShapes(entity);

		Rigidbody3DCreateInfo createInfo(
			transform.GetPosition(TransformSpace::Global),
			transform.GetRotation(TransformSpace::Global),
			collision.front(),
			body._mass,
			body._type);

		body._body = Physics3DService::Get()->GetPlatform().CreateRigidbody(createInfo);
	}
}