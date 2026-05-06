//
// Created by cullen on 4/1/26.
//

#include "UUID.h"

#include <iomanip>
#include <sstream>

#include "Coco/Core/Math/Math.h"
#include "Coco/Core/Math/Random.h"

namespace Coco
{
    const UUID UUID::Nil = UUID();

    UUID::UUID() noexcept :
        Parts{0, 0}
    {}

    UUID::UUID(uint64 firstHalf, uint64 secondHalf) noexcept :
        Parts{firstHalf, secondHalf}
    {}

    UUID::UUID(Span<const uint8, 16> bytes) :
        Raw{}
    {
        memcpy(Raw, bytes.data(), 16);
    }

    UUID::UUID(const UUID& other) noexcept :
        Parts{other.Parts[0], other.Parts[1]}
    {}

    UUID::UUID(UUID&& other) noexcept :
        UUID()
    {
        swap(*this, other);
    }

    UUID& UUID::operator=(UUID other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    UUID& UUID::operator=(UUID&& other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    UUID UUID::New()
    {
        uint64 partOne = Random::RandomUInt64();
        uint64 partTwo = Random::RandomUInt64();
        return {partOne, partTwo};
    }

    String UUID::AsString() const
    {
        std::stringstream stream;

        stream << std::hex;

        for (uint8 i = 0; i < 16; i++)
        {
            stream << std::setw(2) << std::setfill('0') << (int)Raw[i];

            if (i == 3 || i == 5 || i == 7 || i == 9)
                stream << '-';
        }

        return String(stream.str());
    }

    String UUID::AsShortString() const
    {
        std::stringstream stream;
        stream << std::hex;

        for (const uint8 i : Raw)
        {
            stream << std::setw(2) << std::setfill('0') << static_cast<int>(i);
        }

        return String(stream.str());
    }

    bool operator==(const UUID& a, const UUID& b) noexcept
    {
        return a.Parts[0] == b.Parts[0] && a.Parts[1] == b.Parts[1];
    }

    uint64 ToHash(const UUID& uuid) noexcept
    {
        return Math::CombineHashes(uuid.Parts[0], uuid.Parts[1]);
    }

    String ToString(const UUID& uuid)
    {
        return uuid.AsString();
    }

    void swap(UUID& a, UUID& b) noexcept
    {
        using std::swap;
        swap(a.Parts, b.Parts);
    }
} // Coco