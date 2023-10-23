#pragma once
#include <Coco/Core/Types/Transform.h>

namespace Coco::ECS
{
    /// @brief A component that stores a 3D transform
    struct Transform3DComponent
    {
        /// @brief The 3D transform
        Transform3D Transform;

        /// @brief If true, this transform will move relative to its parent
        bool InheritParentTransform;

        /// @brief If true, this transform should be recalculated before it is used
        bool IsDirty;

        Transform3DComponent();
        Transform3DComponent(const Transform3D& transform, bool inheritParentTransform = true);
        Transform3DComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool inheritParentTransform = true);
    };
}