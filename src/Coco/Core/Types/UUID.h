//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_UUID_H
#define COCOENGINE_UUID_H
#include "CoreTypes.h"
#include "Span.h"
#include "String.h"

namespace Coco
{
    struct UUID
    {
        union
        {
            uint64 Parts[2];
            uint8 Raw[16];
        };

        static const UUID Nil;

        UUID() noexcept;
        UUID(uint64 firstHalf, uint64 secondHalf) noexcept;
        explicit UUID(Span<const uint8, 16> bytes);
        UUID(const UUID& other) noexcept;
        UUID(UUID&& other) noexcept;

        UUID& operator=(UUID other) noexcept;
        UUID& operator=(UUID&& other) noexcept;

        static UUID New();

        friend void swap(UUID& a, UUID& b) noexcept;

        String AsString() const;
        String AsShortString() const;
    };

    bool operator==(const UUID& a, const UUID& b) noexcept;
    inline bool operator!=(const UUID& a, const UUID& b) noexcept { return !(a == b); }

    uint64 ToHash(const UUID& uuid) noexcept;
    String ToString(const UUID& uuid);
} // Coco

namespace std
{
    template<>
    struct hash<Coco::UUID>
    {
        size_t operator()(const Coco::UUID& uuid) const noexcept
        {
            return Coco::ToHash(uuid);
        }
    };
}

#endif //COCOENGINE_UUID_H