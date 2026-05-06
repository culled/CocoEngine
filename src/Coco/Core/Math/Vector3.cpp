//
// Created by cullen on 3/7/26.
//

#include "Vector3.h"

namespace Coco
{
    template<>
    const Vector3i Vector3i::Zero = Vector3i(0, 0, 0);

    template<>
    const Vector3i Vector3i::Right = Vector3i(1, 0, 0);

    template<>
    const Vector3i Vector3i::Left = Vector3i(-1, 0, 0);

    template<>
    const Vector3i Vector3i::Up = Vector3i(0, 1, 0);

    template<>
    const Vector3i Vector3i::Down = Vector3i(0, -1, 0);

    template<>
    const Vector3i Vector3i::Backward = Vector3i(0, 0, 1);

    template<>
    const Vector3i Vector3i::Forward = Vector3i(0, 0, -1);

    template<>
    const Vector3i Vector3i::One = Vector3i(1, 1, 1);

    template<>
    const Vector3 Vector3::Zero = Vector3(0.f, 0.f, 0.f);

    template<>
    const Vector3 Vector3::Right = Vector3(1.f, 0.f, 0.f);

    template<>
    const Vector3 Vector3::Left = Vector3(-1.f, 0.f, 0.f);

    template<>
    const Vector3 Vector3::Up = Vector3(0.f, 1.f, 0.f);

    template<>
    const Vector3 Vector3::Down = Vector3(0.f, -1.f, 0.f);

    template<>
    const Vector3 Vector3::Backward = Vector3(0.f, 0.f, 1.f);

    template<>
    const Vector3 Vector3::Forward = Vector3(0.f, 0.f, -1.f);

    template<>
    const Vector3 Vector3::One = Vector3(1.f, 1.f, 1.f);
} // Coco