#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>

namespace Coco
{
	/// @brief A generic stream class
	class COCOAPI Stream
	{
	public:
		virtual ~Stream() = default;

		/// @brief Determines if this stream is still valid
		/// @return 
		virtual bool IsValid() const = 0;

		/// @brief Gets the current position in the stream
		/// @return The position in the stream
		virtual uint64_t GetPosition() = 0;
	};

	/// @brief A generic class that can read streams
	class COCOAPI StreamReader : public Stream
	{
	protected:
		StreamReader() = default;

	protected:
		/// @brief Gets the actual stream object
		/// @return The stream object
		virtual std::istream& GetReadStream() = 0;

	public:
		virtual ~StreamReader() = default;

		/// @brief Seeks the read position to a position in the stream
		/// @param position The absolute position to seek to
		/// @param relative If true, the position will be interpreted as an offset from the current position
		virtual void Seek(uint64_t position, bool relative = false);

		/// @brief Peeks at the current byte in the streak without changing the read position
		/// @return The current byte in the stream
		char Peek();

		/// @brief Peeks at the current line in the stream without changing the read position
		/// @param text The peeked text
		/// @param lineEnd The line terminator character
		virtual void PeekLine(string& text, char lineEnd = '\n');

		/// @brief Reads a line of text
		/// @param text The text that was read
		/// @param lineEnd The line terminator
		/// @return True if a line was successfully read
		virtual bool ReadLine(string& text, char lineEnd = '\n');

		/// @brief Reads text
		/// @param charactersToRead The maximum number of characters to read (may be less if the stream ends prematurely)
		/// @return The text that was read
		virtual string ReadText(uint64_t charactersToRead);

		/// @brief Reads bytes
		/// @param bytesToRead The maximum number of bytes to read (may be less if the stream ends prematurely)
		/// @return The bytes that were read
		virtual List<char> Read(uint64_t bytesToRead);
	};

	/// @brief A generic class that can write to streams
	class COCOAPI StreamWriter : public Stream
	{
	protected:
		StreamWriter() = default;

		/// @brief Gets the output stream object
		/// @return The output stream object
		virtual std::ostream& GetWriteStream() = 0;

	public:
		/// @brief Writes bytes
		/// @param bytes The bytes to write
		virtual void Write(const List<char>& bytes);

		/// @brief Writes text
		/// @param text The text to write
		virtual void WriteText(const string& text);

		/// @brief Writes a line of text
		/// @param text The text
		/// @param lineEnd The line terminator that will be written
		virtual void WriteLine(const string& text, char lineEnd = '\n');

		/// @brief Flushes unwritten changes to the stream
		virtual void Flush();
	};
}