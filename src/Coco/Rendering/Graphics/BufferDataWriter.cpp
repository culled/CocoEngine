#include "Renderpch.h"
#include "BufferDataWriter.h"

#include "GraphicsDevice.h"
#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	BufferDataWriter::BufferDataWriter(const BufferDataLayout& layout, const GraphicsDevice& device) :
		_device(device),
		_layout(layout),
		_data(layout.GetTotalSize(device))
	{}

	void BufferDataWriter::WriteElement(uint64 elementIndex, uint64 arrayIndex, const void* data, uint64 dataSize)
	{
		const BufferDataElement& e = _layout.Elements.at(elementIndex);
		uint64 size = 0;
		uint64 alignment = 0;
		BufferDataLayout::GetElementSpecs(e, _device, size, alignment);

		Assert(size == dataSize)

		uint64 offset = _layout.GetElementBufferOffset(_device, elementIndex, arrayIndex);
		Assert(offset + size <= _data.size())

		Assert(memcpy_s(_data.data() + offset, _data.size(), data, dataSize) == 0);
	}

	void BufferDataWriter::WriteSubElement(uint64 elementIndex, uint64 arrayIndex, uint64 subElementIndex, const void* data, uint64 dataSize)
	{
		const BufferDataElement& e = _layout.Elements.at(elementIndex);
		BufferDataType d = e.SubElements.at(subElementIndex);
		uint64 size = GetBufferDataTypeSize(d);

		Assert(size == dataSize)

		uint64 offset = _layout.GetSubElementBufferOffset(_device, elementIndex, arrayIndex, subElementIndex);
		Assert(offset + size <= _data.size())

		Assert(memcpy_s(_data.data() + offset, _data.size(), data, dataSize) == 0);
	}
}