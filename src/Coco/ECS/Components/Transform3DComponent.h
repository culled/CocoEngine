#pragma once
#include <Coco/Core/Types/Transform.h>

namespace Coco::ECS
{
    /// @brief A component that stores a 3D transform
    struct Transform3DComponent
    {
        /// @brief The 3D transform
        Transform3D Transform;

        Transform3DComponent();
        Transform3DComponent(const Transform3D& transform);
        Transform3DComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
    };
}