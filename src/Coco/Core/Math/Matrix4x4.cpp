//
// Created by cullen on 3/26/26.
//

#include "Matrix4x4.h"
namespace Coco
{
    template<>
    const Matrix4x4 Matrix4x4::Identity = Matrix4x4(
        std::array<float, 16> {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        });
}
