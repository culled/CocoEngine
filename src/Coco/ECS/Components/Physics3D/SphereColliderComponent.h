#pragma once
#include "ColliderComponent.h"

namespace Coco::ECS
{
    class SphereColliderComponent :
        public ColliderComponent
    {
        friend class SphereColliderComponentSerializer;

    private:
        double _radius;

    public:
        SphereColliderComponent(const Entity& owner, double radius = 0.5);

        SharedRef<Physics3D::CollisionShape3D> GetCollisionShape() const override;

        void SetRadius(double radius);
        double GetRadius() const { return _radius; }
    };
}