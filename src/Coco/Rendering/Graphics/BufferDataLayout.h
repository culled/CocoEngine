#pragma once

#include "../Renderpch.h"
#include "BufferTypes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// @brief An element in a BufferDataLayout
	struct BufferDataElement
	{
		/// @brief The data types that compose this element
		std::vector<BufferDataType> SubElements;

		/// @brief The number of elements in this array (set to 1 for a single element)
		uint64 Count;

		BufferDataElement(const std::vector<BufferDataType>& subElements, uint64 count);
	};

	/// @brief Represents a collection of elements in a Buffer
	struct BufferDataLayout
	{
		/// @brief The elements in the buffer
		std::vector<BufferDataElement> Elements;

		BufferDataLayout(const std::vector<BufferDataElement>& elements);

		/// @brief Gets the size and alignment of an element
		/// @param element The element
		/// @param device The graphics device
		/// @param outSize Will be set to the total size of the element
		/// @param outAlignment Will be set to the alignment of the element
		static void GetElementSpecs(const BufferDataElement& element, const GraphicsDevice& device, uint64& outSize, uint64& outAlignment);

		/// @brief Gets the size of this layout
		/// @param device The device
		/// @return The total size of this layout
		uint64 GetTotalSize(const GraphicsDevice& device) const;

		/// @brief Gets the offset of an element in this layout
		/// @param device The graphics device
		/// @param elementIndex The index of the element in the Elements vector
		/// @param arrayIndex The index of the element in the element array (must be less than the element's Count)
		/// @return The offset in the buffer of the element
		uint64 GetElementBufferOffset(const GraphicsDevice& device, uint64 elementIndex, uint64 arrayIndex) const;

		/// @brief Gets the offset of a element's subelement in this layout
		/// @param device The graphics device
		/// @param elementIndex The index of the element in the Elements vector
		/// @param arrayIndex The index of the element in the element array (must be less than the element's Count)
		/// @param subElementIndex The index of the sub element within the element's SubElements vector
		/// @return The offset in the buffer of the element's subelement
		uint64 GetSubElementBufferOffset(const GraphicsDevice& device, uint64 elementIndex, uint64 arrayIndex, uint64 subElementIndex) const;
	};
}