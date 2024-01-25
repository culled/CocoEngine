#pragma once
#include "../EntityComponent.h"

#include <Coco/Physics3D/Rigidbody3D.h>

namespace Coco::ECS
{
    class Rigidbody3DComponent :
        public EntityComponent
    {
        friend class Physics3DSystem;
        friend class Rigidbody3DComponentSerializer;

    private:
        Ref<Physics3D::Rigidbody3D> _body;

        double _mass;
        Physics3D::Rigidbody3DType _type;
        bool _interpolate;

    public:
        Rigidbody3DComponent(const Entity& owner);
        Rigidbody3DComponent(const Entity& owner, double mass, Physics3D::Rigidbody3DType type);

        void SetMass(double mass);
        double GetMass() const { return _mass; }

        void SetType(Physics3D::Rigidbody3DType type);
        Physics3D::Rigidbody3DType GetType() const { return _type; }

        void GetWorldTransform(Vector3& outPosition, Quaternion& outRotation) const;

        bool IsAwake() const;

        Vector3 GetLinearVelocity() const;
        Vector3 GetAngularVelocity() const;
    };
}