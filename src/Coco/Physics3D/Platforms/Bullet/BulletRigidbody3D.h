#pragma once

#include "../../Rigidbody3D.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Transform.h>
#include "../../CollisionShape3D.h"

#include <btBulletDynamicsCommon.h>

namespace Coco::Physics3D::Bullet
{
    class BulletRigidbody3D :
        public Rigidbody3D
    {
    private:
        UniqueRef<btMotionState> _motionState;
        UniqueRef<btRigidBody> _body;
        UniqueRef<btCollisionShape> _collisionShape;
        std::vector<UniqueRef<btCollisionShape>> _baseShapes;
        btVector3 _localInertia;
        double _mass;
        Rigidbody3DType _type;

    public:
        BulletRigidbody3D(const Rigidbody3DCreateInfo& createInfo);
        ~BulletRigidbody3D();

        void SetMass(double mass) override;
        double GetMass() const override { return _mass; }

        void SetType(Rigidbody3DType type) override;
        Rigidbody3DType GetType() const override { return _type; }

        void SetTransform(const Vector3& position, const Quaternion& rotation) override;
        void GetTransform(Vector3& outPosition, Quaternion& outRotation, bool withInterpolation) const override;

        bool IsAwake() const override;

        Vector3 GetLinearVelocity() const override;
        Vector3 GetAngularVelocity() const override;

        btRigidBody* GetBody() const { return _body.get(); }

    private:
        void BuildCollisionShape(std::span<SharedRef<CollisionShape3D>> collisionShapes);

        void UpdateBodyState();
    };
}