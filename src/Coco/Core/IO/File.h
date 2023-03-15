#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "FileTypes.h"

#include <fstream>

namespace Coco
{
	class COCOAPI File
	{
	private:
		FileModes _modes;
		string _path;
		std::fstream _handle;

	public:
		File(const string& path, FileModes openModes);
		File(File&&) noexcept;
		virtual ~File();

		File& operator=(File&&) noexcept;

		File() = delete;
		File(const File&) = delete;

		File& operator=(const File&) = delete;

		/// <summary>
		/// Checks if a file exists
		/// </summary>
		/// <param name="path">The path of the file</param>
		/// <returns>True if the file exists</returns>
		static bool Exists(const string& path);

		/// <summary>
		/// Opens a file
		/// </summary>
		/// <param name="path">The file to open</param>
		/// <param name="openModes">Mode flags to open the file with</param>
		/// <returns>The opened file</returns>
		static File Open(const string& path, FileModes openModes) { return File(path, openModes); }

		/// <summary>
		/// Reads a file's contents as text
		/// </summary>
		/// <param name="path">The file to read</param>
		/// <returns>The contents of the file as text</returns>
		static string ReadAllText(const string& path);

		/// <summary>
		/// Reads a file's contents as binary data
		/// </summary>
		/// <param name="path">The file to read</param>
		/// <returns>The contents of the file as binary data</returns>
		static List<uint8_t> ReadAllBytes(const string& path);

		/// <summary>
		/// Gets the path of this file
		/// </summary>
		/// <returns>The file's path</returns>
		string GetFilePath() const { return _path; }

		/// <summary>
		/// Returns the byte at the current position of the file without moving the read position
		/// </summary>
		/// <returns>The byte at the current position</returns>
		uint8_t Peek();

		/// <summary>
		/// Reads a number of bytes from this file until it reaches the target number of bytes or the end of the file.
		/// This advances the internal read position
		/// </summary>
		/// <param name="bytesToRead">The number of bytes to read</param>
		/// <returns>The binary data that was read (could be shorter than the number of bytes given to read)</returns>
		List<uint8_t> Read(uint64_t bytesToRead);

		/// <summary>
		/// Reads the remainder of the file as binary data
		/// </summary>
		/// <returns>The remaining binary data</returns>
		List<uint8_t> ReadToEnd();

		/// <summary>
		/// Writes a number of bytes to this file
		/// </summary>
		/// <param name="bytes">The bytes to write</param>
		void Write(const List<uint8_t>& bytes);

		/// <summary>
		/// Reads a number of text characters from this file until it reaches the target number of characters or the end of the file.
		/// This advances the internal read position
		/// </summary>
		/// <param name="charactersToRead">The number of characters to read</param>
		/// <returns>The read text</returns>
		string ReadText(uint64_t charactersToRead);

		/// <summary>
		/// Reads the remainder of the file as text
		/// </summary>
		/// <returns>The remaining file as text</returns>
		string ReadTextToEnd();

		/// <summary>
		/// Writes text to this file
		/// </summary>
		/// <param name="text">The text to write</param>
		void WriteText(const string& text);

		/// <summary>
		/// Reads a line of text from this file, deliminated by the given deliminator.
		/// The deliminator will not be included in the read text
		/// </summary>
		/// <param name="text">A string where the read text will be placed</param>
		/// <param name="lineEnd">The deliminator for the end of line</param>
		/// <returns>True if end of file has not been reached</returns>
		bool ReadLine(string& text, char lineEnd = '\n');

		/// <summary>
		/// Writes a line of text to the file
		/// </summary>
		/// <param name="text">The text to write</param>
		void WriteLine(const string& text);

		/// <summary>
		/// Seeks the position to an absolute position within the file
		/// </summary>
		/// <param name="position">The position (in bytes) to seek the position to</param>
		void Seek(uint64_t position);

		/// <summary>
		/// Seeks the position a number of bytes before or after its current position
		/// </summary>
		/// <param name="positionOffset">The number of bytes to move the position</param>
		void SeekRelative(int64_t positionOffset);

		/// <summary>
		/// Gets the current position within the file
		/// </summary>
		/// <returns>The current position</returns>
		uint64_t GetPosition();

		/// <summary>
		/// Gets the size of this file
		/// </summary>
		/// <returns>The file size (in bytes)</returns>
		uint64_t GetSize();

		/// <summary>
		/// Flushes unwritten changes to the file
		/// </summary>
		void Flush();

		/// <summary>
		/// Closes this file
		/// </summary>
		void Close();

	private:
		/// <summary>
		/// Checks if the internal handle is valid
		/// </summary>
		void CheckHandle() const;
	};
}