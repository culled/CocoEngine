//
// Created by cullen on 5/2/26.
//

#include "Transform2DComponent.h"

#include "Coco/ECS/Entity.h"
#include "Coco/ECS/EntityChildView.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Transform2DComponent, EntityComponent);

    Transform2DComponent::Transform2DComponent(const UUID& ownerEntityID) :
        EntityComponent(ownerEntityID),
        LocalPosition(),
        LocalRotation(0.0f),
        LocalScale(Vector2::One),
        InheritParentTransform(true),
        GlobalTransform(Matrix4x4::Identity)
    {}

    Transform2DComponent::Transform2DComponent(const UUID& ownerEntityID, const Vector2& position,
        float rotation, const Vector2& scale) :
        EntityComponent(ownerEntityID),
        LocalPosition(position),
        LocalRotation(rotation),
        LocalScale(scale),
        InheritParentTransform(true),
        GlobalTransform()
    {
        RecalculateGlobalTransform(false);
    }

    Transform2DComponent* Transform2DComponent::GetParentTransform()
    {
        Entity owner = GetOwner();
        if (!owner)
            return nullptr;

        Entity parent = owner.GetParent();
        if (!parent)
            return nullptr;

        return parent.GetComponent<Transform2DComponent>();
    }

    void Transform2DComponent::RecalculateGlobalTransform(bool recalculateChildren)
    {
        Matrix4x4 localTransform = Matrix4x4::CreateTransform(
            Vector3(LocalPosition, 0.0f),
            Quaternion(Vector3(0.0f, 0.0f, LocalRotation)),
            Vector3(LocalScale, 1.0f)
        );
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
            if (Transform2DComponent* childTransform = child.GetComponent<Transform2DComponent>())
                childTransform->RecalculateGlobalTransform(true);
        }
    }

    Vector2 Transform2DComponent::GetGlobalPosition() const
    {
        return GlobalTransform.GetTranslation().XY();
    }

    float Transform2DComponent::GetGlobalRotation() const
    {
        // TODO: this can be optimized
        return GlobalTransform.GetRotation().GetEulerAngles().Z();
    }

    Vector2 Transform2DComponent::TransformVector(const Vector2& vector) const
    {
        return (GlobalTransform * Vector4(vector, 0.0f, 0.0f)).XY();
    }

    Vector2 Transform2DComponent::InverseTransformVector(const Vector2& vector) const
    {
        return (GlobalTransform.Inverse() * Vector4(vector, 0.0f, 0.0f)).XY();
    }

    Vector2 Transform2DComponent::TransformPosition(const Vector2& position) const
    {
        return (GlobalTransform * Vector4(position, 0.0f, 1.0f)).XY();
    }

    Vector2 Transform2DComponent::InverseTransformPosition(const Vector2& position) const
    {
        return (GlobalTransform.Inverse() * Vector4(position, 0.0f, 1.0f)).XY();
    }

    Rect Transform2DComponent::TransformRect(const Rect& rect) const
    {
        Vector2 sizeVec(rect.Size.Width, rect.Size.Height);
        Vector2 transformedSize = TransformVector(sizeVec);
        return {TransformPosition(rect.Offset), Size(transformedSize.X(), transformedSize.Y())};
    }

    Rect Transform2DComponent::InverseTransformRect(const Rect& rect) const
    {
        Vector2 sizeVec(rect.Size.Width, rect.Size.Height);
        Vector2 transformedSize = InverseTransformVector(sizeVec);
        return {InverseTransformPosition(rect.Offset), Size(transformedSize.X(), transformedSize.Y())};
    }
} // Coco