//
// Created by cullen on 3/1/26.
//

#include "Vector2.h"

namespace Coco
{
    template<>
    const Vector2i Vector2i::Zero = Vector2i(0, 0);

    template<>
    const Vector2i Vector2i::Right = Vector2i(1, 0);

    template<>
    const Vector2i Vector2i::Left = Vector2i(-1, 0);

    template<>
    const Vector2i Vector2i::Up = Vector2i(0, 1);

    template<>
    const Vector2i Vector2i::Down = Vector2i(0, -1);

    template<>
    const Vector2i Vector2i::One = Vector2i(1, 1);

    template<>
    const Vector2 Vector2::Zero = Vector2(0.f, 0.f);

    template<>
    const Vector2 Vector2::Right = Vector2(1.f, 0.f);

    template<>
    const Vector2 Vector2::Left = Vector2(-1.f, 0.f);

    template<>
    const Vector2 Vector2::Up = Vector2(0.f, 1.f);

    template<>
    const Vector2 Vector2::Down = Vector2(0.f, -1.f);

    template<>
    const Vector2 Vector2::One = Vector2(1.f, 1.f);

    uint64 ToHash(const Vector2i& vector) noexcept
    {
        return Math::CombineHashes(static_cast<uint64>(vector.X()), static_cast<uint64>(vector.Y()));
    }
}
