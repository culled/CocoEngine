#pragma once
#include "./ColliderComponent.h"

#include <Coco/Core/Types/Vector.h>

namespace Coco::ECS
{
    class BoxColliderComponent :
        public ColliderComponent
    {
        friend class BoxColliderComponentSerializer;

    private:
        Vector3 _size;

    public:
        BoxColliderComponent(const Entity& owner, const Vector3& size = Vector3::One);

        SharedRef<Physics3D::CollisionShape3D> GetCollisionShape() const override;

        void SetSize(const Vector3& size);
        const Vector3& GetSize() const { return _size; }
    };
}