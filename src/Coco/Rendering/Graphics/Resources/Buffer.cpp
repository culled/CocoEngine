#include "Buffer.h"

namespace Coco::Rendering
{
	Buffer::Buffer(const ResourceID& id, const string& name, BufferUsageFlags usageFlags) noexcept : RenderingResource(id, name),
		UsageFlags(usageFlags)
	{}
}
