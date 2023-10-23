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

	/// @brief Base class for streams that can write data
	struct OutputStream : virtual public Stream
	{
		virtual ~OutputStream() = default;

		/// @brief Flushes pending changes to the underlying stream device
		virtual void Flush() = 0;
	};
}