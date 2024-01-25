#pragma once

#include "GraphicsResource.h"
#include "BufferTypes.h"
#include <Coco/Core/Types/Refs.h>

namespace Coco::Rendering
{
	class GraphicsDevice;

	class Buffer :
		public GraphicsResource
	{
	public:
		virtual ~Buffer() = default;

		static Ref<Buffer> CreateStaging(GraphicsDevice& device, uint64 size);

		virtual BufferUsageFlags GetUsageFlags() const = 0;
		virtual bool IsHostVisible() const = 0;
		virtual uint64 GetSize() const = 0;
		virtual void LoadData(uint64 offset, const void* data, uint64 dataSize) = 0;
		virtual void* GetMappedData() const = 0;
		virtual void CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size) const = 0;

	protected:
		Buffer(const GraphicsResourceID& id);
	};
}