#pragma once
#include "../../PhysicsWorld3D.h"

#include <Coco/Core/Types/Refs.h>

#include <btBulletDynamicsCommon.h>

namespace Coco::Physics3D::Bullet
{
    class BulletPhysicsWorld3D :
        public PhysicsWorld3D
    {
    private:
        UniqueRef<btDiscreteDynamicsWorld> _world;
        UniqueRef<btCollisionConfiguration> _collisionConfig;
        UniqueRef<btDispatcher> _dispatcher;
        UniqueRef<btBroadphaseInterface> _overlappingPairCache;
        UniqueRef<btConstraintSolver> _solver;

        double _fixedTimestep;
        int _maxSubsteps;

    public:
        BulletPhysicsWorld3D(const Vector3& gravity = Vector3(0, -9.81, 0), double fixedTimestep = 1.0 / 60.0);
        ~BulletPhysicsWorld3D();

        void Simulate(double deltaTime) override;

        void SetGravity(const Vector3& gravity) override;
        Vector3 GetGravity() const override;

        void SetFixedTimestep(double deltaTime) override;
        double GetFixedTimestep() const override { return _fixedTimestep; }

        void AddRigidbody(Ref<Rigidbody3D> rigidbody) override;
        void RemoveRigidbody(Ref<Rigidbody3D> rigidbody) override;
    };
}