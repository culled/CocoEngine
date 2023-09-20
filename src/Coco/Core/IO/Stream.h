#pragma once

#include "../Corepch.h"
#include "../Defines.h"

namespace Coco
{
	/// @brief Base class for all streams
	struct Stream
	{
		virtual ~Stream() = default;

		/// @brief Gets the current position in the stream, if supported
		/// @return The current position in the stream
		virtual uint64 GetPosition() const = 0;

		/// @brief Seeks the current position to a position within the stream
		/// @param position The new position
		/// @param relative If true, the position will be moved from where it currently is. If false, the position will seek from the start of the stream
		virtual void Seek(uint64 position, bool relative = false) = 0;
	};

	/// @brief Base class for streams that can be read from
	struct InputStream : public Stream
	{
		virtual ~InputStream() = default;

		/// @brief Peeks the byte that is at the current stream position, without moving the stream position
		/// @return The byte
		virtual uint8 Peek() = 0;

		/// @brief Reads data from this stream into a given span of data
		/// @param data The span to read data into
		/// @return The number of bytes read
		virtual uint64 Read(std::span<uint8> data) = 0;
	};

	/// @brief Base class for streams that can write data
	struct OutputStream : public Stream
	{
		virtual ~OutputStream() = default;

		/// @brief Writes a span of data to this stream
		/// @param data The data to write
		virtual void Write(std::span<const uint8> data) = 0;

		/// @brief Flushes pending changes to the underlying stream device
		virtual void Flush() = 0;
	};
}