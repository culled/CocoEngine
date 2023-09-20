#pragma once

#include "Stream.h"
#include "../Types/String.h"

namespace Coco
{
	/// @brief An input stream that can read text
	struct InputTextStream : public InputStream
	{
		virtual ~InputTextStream() = default;

		/// @brief Peeks a single character without moving the stream position
		/// @return The peeked character
		virtual char PeekChar() = 0;

		/// @brief Peeks a single line from the stream without moving the stream position
		/// @param outText Will be set to the peeked text 
		/// @param lineTerminator The line terminator character
		virtual void PeekLine(string& outText, char lineTerminator = '\n') = 0;

		/// @brief Reads a number of characters as text
		/// @param maxLength The maximum number of characters to read
		/// @return The read text
		virtual string ReadText(uint64 maxLength) = 0;

		/// @brief Reads a line from a stream
		/// @param outText Will be set to the read text
		/// @param lineTerminator The line terminator character
		/// @return True if any text was read
		virtual bool ReadLine(string& outText, char lineTerminator = '\n') = 0;
	};

	/// @brief An output stream that can write text
	struct OutputTextStream : public OutputStream
	{
		virtual ~OutputTextStream() = default;

		/// @brief Writes text to this stream
		/// @param text The text to write
		virtual void Write(const string& text) = 0;

		/// @brief Writes a line of text to this stream
		/// @param text The text to write
		/// @param lineTerminator The line terminator to add after the text
		virtual void WriteLine(const string& text, char lineTerminator = '\n') = 0;
	};
}