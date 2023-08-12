#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "FileTypes.h"
#include "Stream.h"

#include <fstream>

namespace Coco
{
	/// @brief A file that can read and write data
	class COCOAPI File : public StreamReader, public StreamWriter
	{
	public:
		/// @brief The path of the file
		const string Path;

	private:
		FileModeFlags _openFlags;
		std::fstream _handle;

	public:
		File(const string& path, FileModeFlags openFlags);
		File(File&&) noexcept;

		File() = delete;
		File(const File&) = delete;
		File& operator=(const File&) = delete;
		File& operator=(File&&) = delete;

		virtual ~File();

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
		static List<char> ReadAllBytes(const string& path);

		/// @brief Writes text to a file
		/// @param path The file to write to
		/// @param text The text to write
		static void WriteAllText(const string& path, const string& text);

		/// @brief Writes binary data to a file
		/// @param path The file to write to
		/// @param data The binary data to write
		static void WriteAllBytes(const string& path, const List<char>& data);

		uint64_t GetPosition() final;
		bool IsValid() const final { return _handle.is_open(); }

		/// @brief Reads the remaining bytes in this file
		/// @return The remaining bytes in this file
		List<char> ReadToEnd();

		/// @brief Reads the remaining text in this file
		/// @return The remaining text in this file
		string ReadTextToEnd();

		/// @brief Gets the size of this file
		/// @return The file size (in bytes)
		uint64_t GetSize();

		/// @brief Closes this file
		void Close();

	protected:
		std::istream& GetReadStream() final;
		std::ostream& GetWriteStream() final;

	private:
		/// @brief Checks if the internal file handle is valid
		void CheckHandle() const;
	};
}