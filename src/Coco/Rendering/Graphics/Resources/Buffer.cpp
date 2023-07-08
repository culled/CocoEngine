#include "Buffer.h"

namespace Coco::Rendering
{
	Buffer::Buffer(ResourceID id, const string& name, BufferUsageFlags usageFlags) noexcept : RenderingResource(id, name),
		UsageFlags(usageFlags)
	{}
}
