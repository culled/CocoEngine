#include "UUID.h"

namespace Coco
{
    const UUID UUID::Nil = UUID("00000000-0000-0000-0000-000000000000");
    const UUID UUID::Max = UUID("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF");

    UUIDv4::UUIDGenerator<std::mt19937_64> UUID::_generator;

    UUID::UUID() : _uuid()
    {}

    UUID::UUID(const UUID& other) : _uuid(other._uuid)
    {}

    UUID::UUID(uint64_t a, uint64_t b) : _uuid(a, b)
    {}

    UUID::UUID(const string& str) :
        _uuid(UUIDv4::UUID::fromStrFactory(str.c_str()))
    {}

    UUID::UUID(const UUIDv4::UUID& uuid) : _uuid(uuid)
    {}

    UUID UUID::CreateV4()
    {
        return UUID(_generator.getUUID());
    }
}
