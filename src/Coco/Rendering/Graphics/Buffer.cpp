#include "Renderpch.h"
#include "Buffer.h"

#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	Buffer::Buffer(const GraphicsResourceID& id) :
		GraphicsResource(id)
	{}

	Ref<Buffer> Buffer::CreateStaging(GraphicsDevice& device, uint64 size)
	{
		return device.CreateBuffer(
			size,
			BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource);
	}
}