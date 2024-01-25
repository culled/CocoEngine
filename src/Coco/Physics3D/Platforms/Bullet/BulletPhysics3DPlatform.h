#pragma once
#include "../../Physics3DPlatform.h"

#include "BulletPhysicsWorld3D.h"
#include "BulletRigidbody3D.h"

namespace Coco::Physics3D::Bullet
{
    class BulletPhysics3DPlatform :
        public Physics3DPlatform
    {
    private:
        std::vector<ManagedRef<BulletPhysicsWorld3D>> _worlds;
        std::vector<ManagedRef<BulletRigidbody3D>> _bodies;

    public:
        // Inherited via Physics3DPlatform
        Ref<PhysicsWorld3D> CreateWorld() override;
        void TryDestroyWorld(Ref<PhysicsWorld3D>& world) override;
        Ref<Rigidbody3D> CreateRigidbody(const Rigidbody3DCreateInfo& createInfo) override;
        void TryDestroyRigidbody(Ref<Rigidbody3D>& rigidbody) override;
    };
}