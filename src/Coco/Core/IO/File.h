#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "FileTypes.h"

#include <fstream>

namespace Coco
{
	/// @brief A file that can read and write data
	class COCOAPI File
	{
	private:
		FileModeFlags _openFlags;
		string _path;
		std::fstream _handle;

	public:
		File(const string& path, FileModeFlags openFlags);
		virtual ~File();

		File(File&&) noexcept;
		File& operator=(File&&) noexcept;

		File() = delete;
		File(const File&) = delete;
		File& operator=(const File&) = delete;

		/// @brief Checks if a file exists
		/// @param path The path of the file
		/// @return True if the file exists
		static bool Exists(const string& path);

		/// @brief Opens a file
		/// @param path The file to open
		/// @param openModeFlags Flags to open the file with
		/// @return The opened file
		static File Open(const string& path, FileModeFlags openModeFlags) { return File(path, openModeFlags); }

		/// @brief Reads a file's contents as text
		/// @param path The file to read
		/// @return The contents of the file as text
		static string ReadAllText(const string& path);

		/// @brief Reads a file's contents as binary data
		/// @param path The file to read
		/// @return The contents of the file as binary data
		static List<uint8_t> ReadAllBytes(const string& path);

		/// @brief Writes text to a file
		/// @param path The file to write to
		/// @param text The text to write
		static void WriteAllText(const string& path, const string& text);

		/// @brief Writes binary data to a file
		/// @param path The file to write to
		/// @param data The binary data to write
		static void WriteAllBytes(const string& path, const List<uint8_t>& data);
		
		/// @brief Gets the path of this file
		/// @return The file's path
		const string& GetFilePath() const { return _path; }

		/// @brief Returns the byte at the current position of the file without moving the read position
		/// @return The byte at the current position
		uint8_t Peek();

		/// @brief Reads a number of bytes from this file until it reaches the target number of bytes or the end of the file
		/// @param bytesToRead The number of bytes to read
		/// @return The binary data that was read
		List<uint8_t> Read(uint64_t bytesToRead);

		/// @brief Reads bytes from the current position to the end of the file
		/// @return The binary data from the current position to the end of the file
		List<uint8_t> ReadToEnd();

		/// @brief Writes bytes to this file
		/// @param bytes The bytes to write
		void Write(const List<uint8_t>& bytes);

		/// @brief Reads a number of text characters from this file until it reaches the target number of characters or the end of the file
		/// @param charactersToRead The number of characters to read
		/// @return The read text
		string ReadText(uint64_t charactersToRead);

		/// @brief Reads text from the current position to the end of the file
		/// @return The text from the current position to the end of the file
		string ReadTextToEnd();

		/// @brief Writes text to this file
		/// @param text The text to write
		void WriteText(const string& text);

		/// @brief Writes a line of text to the file
		/// @param text The text to write
		/// @param lineEnd The deliminator for the end of line
		void WriteLine(const string& text, char lineEnd = '\n');

		/// @brief Reads a line of text from this file, deliminated by the given deliminator. The deliminator will not be included in the read text
		/// @param text A string where the read text will be placed
		/// @param lineEnd The deliminator for the end of line
		/// @return True if a line of text was read
		bool ReadLine(string& text, char lineEnd = '\n');

		/// @brief Peeks a line of text from this file, deliminated by the given deliminator
		/// @param text A string where the read text will be placed
		/// @param lineEnd The deliminator for the end of line
		void PeekLine(string& text, char lineEnd = '\n');

		/// @brief Seeks to a position within the file
		/// @param position The position to seek to (in bytes)
		void Seek(uint64_t position);

		/// @brief Seeks a number of bytes before or after the current position in the file
		/// @param positionOffset The number of bytes to move the position (positive = forward, negative = backward)
		void SeekRelative(int64_t positionOffset);

		/// @brief Gets the current position within the file
		/// @return The current position (in bytes)
		uint64_t GetPosition();

		/// @brief Gets the size of this file
		/// @return The file size (in bytes)
		uint64_t GetSize();

		/// @brief Flushes unwritten changes to the file
		void Flush();

		/// @brief Closes this file
		void Close();

	private:
		/// @brief Checks if the internal file handle is valid
		void CheckHandle() const;
	};
}