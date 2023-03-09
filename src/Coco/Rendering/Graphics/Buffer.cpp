#include "Buffer.h"

namespace Coco::Rendering
{
	Buffer::Buffer(GraphicsDevice* owningDevice, BufferUsageFlags usageFlags) : GraphicsResource(owningDevice),
		UsageFlags(usageFlags)
	{}
}
