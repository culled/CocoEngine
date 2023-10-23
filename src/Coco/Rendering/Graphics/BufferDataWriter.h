#pragma once

#include "BufferDataLayout.h"

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// @brief Basic class for filling out buffer data using a layout
	class BufferDataWriter
	{
	private:
		const GraphicsDevice& _device;
		BufferDataLayout _layout;
		std::vector<uint8> _data;

	public:
		BufferDataWriter(const BufferDataLayout& layout, const GraphicsDevice& device);

		/// @brief Writes raw data for an element
		/// @param elementIndex The index of the element in the layout
		/// @param arrayIndex The index of the element in the element array
		/// @param data The data
		/// @param dataSize The size of the data (must match the element's size)
		void WriteElement(uint64 elementIndex, uint64 arrayIndex, const void* data, uint64 dataSize);

		/// @brief Writes raw data for an element's subelement
		/// @param elementIndex The index of the element in the layout
		/// @param arrayIndex The index of the element in the element array
		/// @param subElementIndex The index of the subelement within the element
		/// @param data The data
		/// @param dataSize The size of the data (must match the buffer data size of the subelement)
		void WriteSubElement(uint64 elementIndex, uint64 arrayIndex, uint64 subElementIndex, const void* data, uint64 dataSize);

		/// @brief Writes data for an element
		/// @tparam ValueType The type of value to write
		/// @param elementIndex The index of the element in the layout
		/// @param arrayIndex The index of the element in the element array 
		/// @param value The value to write
		template<typename ValueType>
		void WriteElement(uint64 elementIndex, uint64 arrayIndex, const ValueType& value)
		{
			WriteElement(elementIndex, arrayIndex, &value, sizeof(ValueType));
		}

		/// @brief Writes data for an element's subelement
		/// @tparam ValueType The type of value to write
		/// @param elementIndex The index of the element in the layout
		/// @param arrayIndex The index of the element in the element array 
		/// @param subElementIndex The index of the subelement within the element
		/// @param value The value to write
		template<typename ValueType>
		void WriteSubElement(uint64 elementIndex, uint64 arrayIndex, uint64 subElementIndex, const ValueType& value)
		{
			WriteSubElement(elementIndex, arrayIndex, subElementIndex, &value, sizeof(ValueType));
		}

		/// @brief Gets the buffer data
		/// @return The buffer data
		std::span<const uint8> GetData() const { return _data; }

		/// @brief Gets the raw buffer data
		/// @return The raw buffer data
		const uint8* GetRawData() const { return _data.data(); }

		/// @brief Gets the size of the buffer data
		/// @return The size of the buffer data
		uint64 GetSize() const { return _data.size(); }
	};
}