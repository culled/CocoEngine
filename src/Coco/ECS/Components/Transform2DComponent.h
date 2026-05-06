//
// Created by cullen on 5/2/26.
//

#ifndef COCOENGINE_TRANSFORM2DCOMPONENT_H
#define COCOENGINE_TRANSFORM2DCOMPONENT_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Vector2.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    struct Transform2DComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(Transform2DComponent)

    public:
        Vector2 LocalPosition;
        float LocalRotation;
        Vector2 LocalScale;
        int ZIndex;
        bool InheritParentTransform;
        Matrix4x4 GlobalTransform;

        Transform2DComponent(const UUID& ownerEntityID);
        Transform2DComponent(const UUID& ownerEntityID, const Vector2& position, float rotation = 0.0f, const Vector2& scale = Vector2::One);

        Transform2DComponent* GetParentTransform();

        void RecalculateGlobalTransform(bool recalculateChildren = true);

        Vector2 GetGlobalPosition() const;
        float GetGlobalRotation() const;

        Vector2 TransformVector(const Vector2& vector) const;
        Vector2 InverseTransformVector(const Vector2& vector) const;

        Vector2 TransformPosition(const Vector2& position) const;
        Vector2 InverseTransformPosition(const Vector2& position) const;
    };
} // Coco

#endif //COCOENGINE_TRANSFORM2DCOMPONENT_H