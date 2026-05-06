//
// Created by cullen on 3/12/26.
//

#include "Vector4.h"

namespace Coco
{
    template<>
    const Vector4 Vector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    template<>
    const Vector4 Vector4::One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    template<>
    const Vector4i Vector4i::Zero = Vector4i(0, 0, 0, 0);
    template<>
    const Vector4i Vector4i::One = Vector4i(1, 1, 1, 1);
} // Coco