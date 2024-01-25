#include "Phys3Dpch.h"
#include "BulletPhysicsWorld3D.h"

#include "BulletRigidbody3D.h"

#include <Coco/Core/Engine.h>

namespace Coco::Physics3D::Bullet
{
	BulletPhysicsWorld3D::BulletPhysicsWorld3D(const Vector3& gravity, double fixedTimestep) :
		_collisionConfig(CreateUniqueRef<btDefaultCollisionConfiguration>()),
		_overlappingPairCache(CreateUniqueRef<btDbvtBroadphase>()),
		_solver(CreateUniqueRef<btSequentialImpulseConstraintSolver>()),
		_fixedTimestep(fixedTimestep),
		_maxSubsteps(10)
	{
		// Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		_dispatcher = CreateUniqueRef<btCollisionDispatcher>(_collisionConfig.get());

		_world = CreateUniqueRef<btDiscreteDynamicsWorld>(_dispatcher.get(), _overlappingPairCache.get(), _solver.get(), _collisionConfig.get());

		SetGravity(gravity);

		CocoTrace("Created BulletPhysicsWorld3D")
	}

	BulletPhysicsWorld3D::~BulletPhysicsWorld3D()
	{
		_world.reset();
		_dispatcher.reset();
		_solver.reset();
		_overlappingPairCache.reset();
		_collisionConfig.reset();

		CocoTrace("Destroyed BulletPhysicsWorld3D")
	}

	void BulletPhysicsWorld3D::Simulate(double deltaTime)
	{
		_world->stepSimulation(static_cast<float>(deltaTime), _maxSubsteps, static_cast<float>(_fixedTimestep));
	}

	void BulletPhysicsWorld3D::SetGravity(const Vector3& gravity)
	{
		_world->setGravity(btVector3(static_cast<float>(gravity.X), static_cast<float>(gravity.Y), static_cast<float>(gravity.Z)));
	}

	Vector3 BulletPhysicsWorld3D::GetGravity() const
	{
		btVector3 gravity = _world->getGravity();

		return Vector3(gravity.x(), gravity.y(), gravity.z());
	}

	void BulletPhysicsWorld3D::SetFixedTimestep(double deltaTime)
	{
		_fixedTimestep = deltaTime;
	}

	void BulletPhysicsWorld3D::AddRigidbody(Ref<Rigidbody3D> rigidbody)
	{
		Assert(rigidbody.IsValid())
		BulletRigidbody3D* rigid = static_cast<BulletRigidbody3D*>(rigidbody.Get());

		_world->addRigidBody(rigid->GetBody());
	}

	void BulletPhysicsWorld3D::RemoveRigidbody(Ref<Rigidbody3D> rigidbody)
	{
		Assert(rigidbody.IsValid())
		BulletRigidbody3D* rigid = static_cast<BulletRigidbody3D*>(rigidbody.Get());

		_world->removeRigidBody(rigid->GetBody());
	}
}