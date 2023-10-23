#pragma once

#include "Stream.h"

namespace Coco
{
	/// @brief Base class for binary streams that can be read from
	struct InputBinaryStream : virtual public Stream
	{
		virtual ~InputBinaryStream() = default;

		/// @brief Peeks the byte that is at the current stream position, without moving the stream position
		/// @return The byte
		virtual uint8 Peek() = 0;

		/// @brief Reads data from this stream into a given span of data
		/// @param data The span to read data into
		/// @return The number of bytes read
		virtual uint64 Read(std::span<uint8> data) = 0;
	};

	/// @brief Base class for binary streams that can write data
	struct OutputBinaryStream : virtual public OutputStream
	{
		virtual ~OutputBinaryStream() = default;

		/// @brief Writes a span of data to this stream
		/// @param data The data to write
		virtual void Write(std::span<const uint8> data) = 0;
	};
}