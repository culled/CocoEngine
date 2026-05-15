//
// Created by cullen on 5/2/26.
//

#ifndef COCOENGINE_TRANSFORM2DCOMPONENT_H
#define COCOENGINE_TRANSFORM2DCOMPONENT_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Rect.h"
#include "Coco/Core/Math/Vector2.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    /// @brief An EntityComponent that defines a position, rotation, and scale in 2D
    struct Transform2DComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(Transform2DComponent)

    public:
        /// @brief The position, relative to the parent
        Vector2 LocalPosition;

        /// @brief The rotation, relative to the parent
        float LocalRotation;

        /// @brief The scale, in this object's space
        Vector2 LocalScale;

        /// @brief The z index. 2D objects are drawn in ascending order
        int ZIndex;

        /// @brief If true, this transform will be relative to the parent transform
        bool InheritParentTransform;

        /// @brief The computed global transform. Transforms points from this object's space into global space
        Matrix4x4 GlobalTransform;

        Transform2DComponent(const UUID& ownerEntityID);
        Transform2DComponent(const UUID& ownerEntityID, const Vector2& position, float rotation = 0.0f, const Vector2& scale = Vector2::One);

        /// @brief Gets this transform's parent transform
        /// @return The parent transform, or nullptr if one doesn't exist
        Transform2DComponent* GetParentTransform();

        /// @brief Recalculates the global transform of this transform, taking into account this transform's position, rotation, scale, and this transform's parent transform (if one exists)
        /// @param recalculateChildren If true, then this transform's child transforms will also be recalculated recursively
        void RecalculateGlobalTransform(bool recalculateChildren = true);

        /// @brief Gets the global position of this transform
        /// @return The global position of this transform
        Vector2 GetGlobalPosition() const;

        /// @brief Gets the global rotation of this transform
        /// @return The global rotation of this transform
        float GetGlobalRotation() const;

        /// @brief Converts a vector from this object's space to global space
        /// @param vector The vector
        /// @return The vector in global space
        Vector2 TransformVector(const Vector2& vector) const;

        /// @brief Converts a vector from global space to this object's space
        /// @param vector The vector
        /// @return The vector in this object's space
        Vector2 InverseTransformVector(const Vector2& vector) const;

        /// @brief Converts a position from this object's space to global space
        /// @param position The position
        /// @return The position in global space
        Vector2 TransformPosition(const Vector2& position) const;

        /// @brief Converts a position from global space to this object's space
        /// @param position The position
        /// @return The position in this object's space
        Vector2 InverseTransformPosition(const Vector2& position) const;

        Rect TransformRect(const Rect& rect) const;
        Rect InverseTransformRect(const Rect& rect) const;
    };
} // Coco

#endif //COCOENGINE_TRANSFORM2DCOMPONENT_H