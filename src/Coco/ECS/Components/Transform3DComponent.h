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
    /// @brief An EntityComponent that defines a position, rotation, and scale in 3D
    struct Transform3DComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(Transform3DComponent)

    public:
        /// @brief The position, relative to the parent
        Vector3 LocalPosition;

        /// @brief The rotation, relative to the parent
        Quaternion LocalRotation;

        /// @brief The scale, in this object's space
        Vector3 LocalScale;

        /// @brief If true, this transform will be relative to the parent transform
        bool InheritParentTransform;

        /// @brief The computed global transform. Transforms points from this object's space into global space
        Matrix4x4 GlobalTransform;

        Transform3DComponent(const UUID& ownerEntityID);
        Transform3DComponent(const UUID& ownerEntityID, const Vector3& position, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);

        /// @brief Gets this transform's parent transform
        /// @return The parent transform, or nullptr if one doesn't exist
        Transform3DComponent* GetParentTransform();

        /// @brief Recalculates the global transform of this transform, taking into account this transform's position, rotation, scale, and this transform's parent transform (if one exists)
        /// @param recalculateChildren If true, then this transform's child transforms will also be recalculated recursively
        void RecalculateGlobalTransform(bool recalculateChildren = true);

        /// @brief Gets the global position of this transform
        /// @return The global position of this transform
        Vector3 GetGlobalPosition() const;

        /// @brief Gets the global rotation of this transform
        /// @return The global rotation of this transform
        Quaternion GetGlobalRotation() const;

        /// @brief Converts a vector from this object's space to global space
        /// @param vector The vector
        /// @return The vector in global space
        Vector3 TransformVector(const Vector3& vector) const;

        /// @brief Converts a vector from global space to this object's space
        /// @param vector The vector
        /// @return The vector in this object's space
        Vector3 InverseTransformVector(const Vector3& vector) const;

        /// @brief Converts a position from this object's space to global space
        /// @param position The position
        /// @return The position in global space
        Vector3 TransformPosition(const Vector3& position) const;

        /// @brief Converts a position from global space to this object's space
        /// @param position The position
        /// @return The position in this object's space
        Vector3 InverseTransformPosition(const Vector3& position) const;
    };
} // Coco

#endif //COCOENGINE_TRANSFORM3DCOMPONENT_H