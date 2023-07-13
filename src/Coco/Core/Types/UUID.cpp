#include "UUID.h"

namespace Coco
{
    UUID CreateUUIDv4()
    {
        UUIDv4::UUIDGenerator<std::mt19937_64> generator;

        return generator.getUUID();
    }
}
