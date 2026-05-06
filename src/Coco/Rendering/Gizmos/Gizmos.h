//
// Created by cullen on 3/31/26.
//

#ifndef COCOENGINE_GIZMOS_H
#define COCOENGINE_GIZMOS_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Types/Color.h"

namespace Coco
{
    /// @brief A draw call for a gizmo shape
    struct GizmoDrawCall
    {
        /// @brief The transform of this shape
        Matrix4x4 Transform;

        /// @brief The color of this shape
        Color DrawColor;

        /// @brief The submesh of the gizmo mesh
        uint8 SubmeshID;

        GizmoDrawCall(
            uint8 submeshID,
            const Color& color,
            const Matrix4x4& transform);
    };

    class Gizmos
    {};
} // Coco

#endif //COCOENGINE_GIZMOS_H