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
    /// @brief A 128-bit universally unique identifier
    struct UUID
    {
        union
        {
            /// @brief The two 64-bit parts
            uint64 Parts[2];

            /// @brief The raw bytes
            uint8 Raw[16];
        };

        /// @brief A UUID with all zeroes (00000000-0000-0000-0000-000000000000)
        static const UUID Nil;

        UUID() noexcept;
        UUID(uint64 firstHalf, uint64 secondHalf) noexcept;
        explicit UUID(Span<const uint8, 16> bytes);
        UUID(const UUID& other) noexcept;
        UUID(UUID&& other) noexcept;

        UUID& operator=(UUID other) noexcept;
        UUID& operator=(UUID&& other) noexcept;

        /// @brief Creates a new random UUID
        /// @return A new UUID
        static UUID New();

        friend void swap(UUID& a, UUID& b) noexcept;

        /// @brief Converts this UUID into a pretty string representation in the format "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
        /// @return The pretty string representation
        String AsString() const;

        /// @brief Converts this UUID into a string representation in the format "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        /// @return The string representation
        String AsShortString() const;
    };

    bool operator==(const UUID& a, const UUID& b) noexcept;
    inline bool operator!=(const UUID& a, const UUID& b) noexcept { return !(a == b); }

    /// @brief Computes the hash value of a UUID
    /// @param uuid The UUID
    /// @return The hash value
    uint64 ToHash(const UUID& uuid) noexcept;

    /// @brief Gets the string representation of a UUID
    /// @param uuid The UUID
    /// @return The string representation
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