#include "Renderpch.h"
#include "BufferDataLayout.h"

#include "GraphicsDevice.h"
#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	BufferDataElement::BufferDataElement(const std::vector<BufferDataType>& subElements, uint64 count) :
		SubElements(subElements),
		Count(count)
	{}

	BufferDataLayout::BufferDataLayout(const std::vector<BufferDataElement>& elements) : 
		Elements(elements)
	{}

	void BufferDataLayout::GetElementSpecs(const BufferDataElement& element, const GraphicsDevice& device, uint64& outSize, uint64& outAlignment)
	{
		uint64 offset = 0;
		uint8 largestSize = 0;

		for (const BufferDataType& d : element.SubElements)
		{
			device.AlignOffset(d, offset);

			uint8 size = GetBufferDataTypeSize(d);
			largestSize = Math::Max(largestSize, size);
			offset += size;
		}

		offset = GraphicsDevice::GetOffsetForAlignment(offset, largestSize);

		outSize = offset;
		outAlignment = largestSize;
	}

	uint64 BufferDataLayout::GetTotalSize(const GraphicsDevice& device) const
	{
		uint64 offset = 0;

		for (const BufferDataElement& e : Elements)
		{
			uint64 size = 0;
			uint64 alignment = 0;
			GetElementSpecs(e, device, size, alignment);

			offset = GraphicsDevice::GetOffsetForAlignment(offset, alignment);
			offset += size * e.Count;
		}

		return offset;
	}

	uint64 BufferDataLayout::GetElementBufferOffset(const GraphicsDevice& device, uint64 elementIndex, uint64 arrayIndex) const
	{
		Assert(elementIndex < Elements.size())

		uint64 offset = 0;
		uint64 currentElementSize = 0;
		uint64 currentElementAlignment = 0;

		for (uint64 i = 0; i <= elementIndex; i++)
		{
			const BufferDataElement& e = Elements.at(i);
			GetElementSpecs(e, device, currentElementSize, currentElementAlignment);

			offset = GraphicsDevice::GetOffsetForAlignment(offset, currentElementAlignment);

			if (i == elementIndex)
			{
				Assert(arrayIndex < e.Count)
				break;
			}

			offset += currentElementSize * e.Count;
		}

		return offset + currentElementSize * arrayIndex;
	}

	uint64 BufferDataLayout::GetSubElementBufferOffset(const GraphicsDevice& device, uint64 elementIndex, uint64 arrayIndex, uint64 subElementIndex) const
	{
		uint64 offset = GetElementBufferOffset(device, elementIndex, arrayIndex);
		const BufferDataElement& e = Elements.at(elementIndex);
		Assert(subElementIndex < e.SubElements.size())

		for (uint64 i = 0; i <= subElementIndex; i++)
		{
			const BufferDataType& d = e.SubElements.at(subElementIndex);
			device.AlignOffset(d, offset);

			if (i == subElementIndex)
				break;

			uint8 size = GetBufferDataTypeSize(d);
			offset += size;
		}

		return offset;
	}
}