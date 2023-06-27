#include "Buffer.h"

namespace Coco::Rendering
{
	Buffer::Buffer(ResourceID id, const string& name, uint64_t lifetime, BufferUsageFlags usageFlags) noexcept : RenderingResource(id, name, lifetime),
		UsageFlags(usageFlags)
	{}
}
