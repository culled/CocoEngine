#include "Phys3Dpch.h"
#include "BulletPhysics3DPlatform.h"

namespace Coco::Physics3D::Bullet
{
	Ref<PhysicsWorld3D> BulletPhysics3DPlatform::CreateWorld()
	{
		_worlds.push_back(CreateManagedRef<BulletPhysicsWorld3D>());
		return _worlds.back();
	}

	void BulletPhysics3DPlatform::TryDestroyWorld(Ref<PhysicsWorld3D>& world)
	{
		auto it = std::find_if(_worlds.begin(), _worlds.end(),
			[world](const ManagedRef<BulletPhysicsWorld3D>& other)
			{
				return other.Get() == world.Get();
			});

		if (it == _worlds.end() || it->GetUseCount() > 2)
			return;

		it->Invalidate();
		_worlds.erase(it);
	}

	Ref<Rigidbody3D> BulletPhysics3DPlatform::CreateRigidbody(const Rigidbody3DCreateInfo& createInfo)
	{
		_bodies.push_back(CreateManagedRef<BulletRigidbody3D>(createInfo));
		return _bodies.back();
	}

	void BulletPhysics3DPlatform::TryDestroyRigidbody(Ref<Rigidbody3D>& rigidbody)
	{
		auto it = std::find_if(_bodies.begin(), _bodies.end(),
			[rigidbody](const ManagedRef<BulletRigidbody3D>& body)
			{
				return body.Get() == rigidbody.Get();
			});

		if (it == _bodies.end() || it->GetUseCount() > 2)
			return;

		it->Invalidate();
		_bodies.erase(it);
	}
}