#pragma once

#include "EntityComponent.h"
#include <Coco/Core/Types/Transform.h>

namespace Coco::ECS
{
    /// @brief A component that stores a 3D transform
    class Transform3DComponent :
        public EntityComponent
    {
        friend class Transform3DComponentSerializer;
        friend class TransformSystem;
        friend class SceneSerializer;

    private:
        Transform3D _transform;
        bool _inheritParentTransform;

    public:
        Transform3DComponent(const Entity& owner);
        Transform3DComponent(const Entity& owner, const Transform3D& transform, bool inheritParentTransform = true);
        Transform3DComponent(const Entity& owner, const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool inheritParentTransform = true);

        /// @brief Gets a matrix that transforms between spaces
        /// @param from The space to transform from
        /// @param to The space to transform to 
        /// @return The transformation matrix
        Matrix4x4 GetTransformMatrix(TransformSpace from, TransformSpace to) const { return _transform.GetTransformMatrix(from, to); }

        /// @brief Gets the internal transform
        /// @return The transform
        const Transform3D& GetTransform() const { return _transform; }

        /// @brief Sets the position in the given space
        /// @param position The position
        /// @param space The space
		void SetPosition(const Vector3& position, TransformSpace space);

        /// @brief Gets the position in the given space
        /// @param space The space
        /// @return The position
        Vector3 GetPosition(TransformSpace space) const { return _transform.GetPosition(space); }

        /// @brief Sets the rotation in the given space
        /// @param rotation The rotation
        /// @param space The space
        void SetRotation(const Quaternion& rotation, TransformSpace space);

        /// @brief Gets the rotation in the given space
        /// @param space The space
        /// @return The rotation
        Quaternion GetRotation(TransformSpace space) const { return _transform.GetRotation(space); }

        /// @brief Sets the rotation in the given space
        /// @param eulerAngles The euler angle rotation, in radians
        /// @param space The space
        void SetEulerAngles(const Vector3& eulerAngles, TransformSpace space) { SetRotation(Quaternion(eulerAngles), space); }

        /// @brief Gets the rotation in euler angles in the given space
        /// @param space The space
        /// @return The euler angle rotation, in radians
        Vector3 GetEulerAngles(TransformSpace space) const { return _transform.GetEulerAngles(space); }

        /// @brief Sets the scale in the given space
        /// @param scale The scale
        /// @param space The space
        void SetScale(const Vector3& scale, TransformSpace space);

        /// @brief Gets the scale in the given space
        /// @param space The space
        /// @return The scale
        Vector3 GetScale(TransformSpace space) const { return _transform.GetScale(space); }

        /// @brief Moves this transform in the given space
        /// @param translation The amount to move
        /// @param space The space to move relative to
        void Translate(const Vector3& translation, TransformSpace space);

        /// @brief Rotates this transform in the given space
        /// @param rotation The amount to rotate
        /// @param space The space to rotate relative to
        void Rotate(const Quaternion& rotation, TransformSpace space);

        /// @brief Rotates this transform in the given space
        /// @param eulerAngles The euler angles to rotate by, in radians
        /// @param space The space to rotate relative to
        void Rotate(const Vector3& eulerAngles, TransformSpace space) { Rotate(Quaternion(eulerAngles), space); }

        /// @brief Rotates this transform in the given space
        /// @param axis The axis to rotate about
        /// @param angleRadians The angle to rotate by
        /// @param space The space to rotate relative to
        void Rotate(const Vector3& axis, double angleRadians, TransformSpace space) { Rotate(Quaternion(axis, angleRadians), space); }

        /// @brief Decomposes this transform
        /// @param space The space
        /// @param outPosition Will be set to the position in the given space
        /// @param outRotation Will be set to the rotation in the given space
        /// @param outScale Will be set to the scale in the given space
        void Decompose(TransformSpace space, Vector3& outPosition, Quaternion& outRotation, Vector3& outScale) const
        {
            return _transform.Decompose(space, outPosition, outRotation, outScale);
        }

        /// @brief Gets a vector that points forward in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetForward(TransformSpace space) const { return _transform.GetForward(space); }

        /// @brief Gets a vector that points backward in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetBackward(TransformSpace space) const { return _transform.GetBackward(space); }

        /// @brief Gets a vector that points right in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetRight(TransformSpace space) const { return _transform.GetRight(space); }

        /// @brief Gets a vector that points left in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetLeft(TransformSpace space) const { return _transform.GetLeft(space); }

        /// @brief Gets a vector that points up in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetUp(TransformSpace space) const { return _transform.GetUp(space); }

        /// @brief Gets a vector that points down in the given space
        /// @param space The space
        /// @return The vector
        Vector3 GetDown(TransformSpace space) const { return _transform.GetDown(space); }

        /// @brief Sets if this transform should inherit its parent's transformation
        /// @param inheritParentTransform The inherit state
        /// @param maintainGlobalTransform If true, this transform will attempt to maintain its world transformation
        void SetInheritParentTransform(bool inheritParentTransform, bool maintainGlobalTransform);

        /// @brief Gets if this transform inherits its parent's transformation
        /// @return True if this transform inherits its parent's transformation
        bool GetInheritParentTransform() const { return _inheritParentTransform; }

        /// @brief Recalculates this transform and its children
        void Recalculate();

	private:
        /// @brief Recalculates this transform and its children
        /// @param parent The parent to calculate relative to, or nullptr for no parent
        void Recalculate(const Transform3D* parent);

        /// @brief Attempts to get the parent transform, if one exists and this transform inherits the parent transform
        /// @param outParentTransform Will be set to the parent transform
        /// @return True if a parent transform was found
        bool TryGetParentTransform(const Transform3D*& outParentTransform) const;
    };
}