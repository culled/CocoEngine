//
// Created by cullen on 4/2/26.
//

#include "Transform3DComponent.h"

#include "Coco/ECS/Entity.h"
#include "Coco/ECS/EntityChildView.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Transform3DComponent, EntityComponent);

    Transform3DComponent::Transform3DComponent(const UUID& ownerEntityID) :
        EntityComponent(ownerEntityID),
        LocalPosition(),
        LocalRotation(Quaternion::Identity),
        LocalScale(Vector3::One),
        InheritParentTransform(true),
        GlobalTransform(Matrix4x4::Identity)
    {}

    Transform3DComponent::Transform3DComponent(const UUID& ownerEntityID, const Vector3& position,
        const Quaternion& rotation, const Vector3& scale) :
        EntityComponent(ownerEntityID),
        LocalPosition(position),
        LocalRotation(rotation),
        LocalScale(scale),
        InheritParentTransform(true),
        GlobalTransform()
    {
        RecalculateGlobalTransform(false);
    }

    Transform3DComponent* Transform3DComponent::GetParentTransform()
    {
        Entity owner = GetOwner();
        if (!owner)
            return nullptr;

        Entity parent = owner.GetParent();
        if (!parent)
            return nullptr;

        return parent.GetComponent<Transform3DComponent>();
    }

    void Transform3DComponent::RecalculateGlobalTransform(bool recalculateChildren)
    {
        Matrix4x4 localTransform = Matrix4x4::CreateTransform(LocalPosition, LocalRotation, LocalScale);
        Matrix4x4 globalParentTransform = Matrix4x4::Identity;

        if (auto parentTransform = GetParentTransform())
            globalParentTransform = parentTransform->GlobalTransform;

        GlobalTransform = globalParentTransform * localTransform;

        if (!recalculateChildren)
            return;

        Entity owner = GetOwner();
        if (!owner)
            return;

        for (auto& child : owner.GetChildren())
        {
            if (Transform3DComponent* childTransform = child.GetComponent<Transform3DComponent>())
                childTransform->RecalculateGlobalTransform(true);
        }
    }

    Vector3 Transform3DComponent::GetGlobalPosition() const
    {
        return GlobalTransform.GetTranslation();
    }

    Quaternion Transform3DComponent::GetGlobalRotation() const
    {
        return GlobalTransform.GetRotation();
    }

    Vector3 Transform3DComponent::TransformVector(const Vector3& vector) const
    {
        return (GlobalTransform * Vector4(vector, 0.0f)).XYZ();
    }

    Vector3 Transform3DComponent::InverseTransformVector(const Vector3& vector) const
    {
        return (GlobalTransform.Inverse() * Vector4(vector, 0.0f)).XYZ();
    }

    Vector3 Transform3DComponent::TransformPosition(const Vector3& position) const
    {
        return (GlobalTransform * Vector4(position, 1.0f)).XYZ();
    }

    Vector3 Transform3DComponent::InverseTransformPosition(const Vector3& position) const
    {
        return (GlobalTransform.Inverse() * Vector4(position, 1.0f)).XYZ();
    }
} // Coco