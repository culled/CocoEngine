//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_TRANSFORM3DCOMPONENT_H
#define COCOENGINE_TRANSFORM3DCOMPONENT_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Quaternion.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    struct Transform3DComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(Transform3DComponent)

    public:
        Vector3 LocalPosition;
        Quaternion LocalRotation;
        Vector3 LocalScale;
        bool InheritParentTransform;
        Matrix4x4 GlobalTransform;

        Transform3DComponent(const UUID& ownerEntityID);
        Transform3DComponent(const UUID& ownerEntityID, const Vector3& position, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);

        Transform3DComponent* GetParentTransform();

        void RecalculateGlobalTransform(bool recalculateChildren = true);

        Vector3 GetGlobalPosition() const;
        Quaternion GetGlobalRotation() const;

        Vector3 TransformVector(const Vector3& vector) const;
        Vector3 InverseTransformVector(const Vector3& vector) const;

        Vector3 TransformPosition(const Vector3& position) const;
        Vector3 InverseTransformPosition(const Vector3& position) const;
    };
} // Coco

#endif //COCOENGINE_TRANSFORM3DCOMPONENT_H