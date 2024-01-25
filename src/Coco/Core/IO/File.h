#pragma once

#include "Stream.h"
#include "BinaryStream.h"
#include "TextStream.h"
#include "FileTypes.h"
#include "FilePath.h"
#include "../Types/CoreExceptions.h"

namespace Coco
{
	/// @brief An exception for errors related to opening files
	class FileOpenException :
		public Exception
	{
	public:
		FileOpenException(const string& message);
	};

	/// @brief An exception for errors related to creating files
	class FileCreateException :
		public Exception
	{
	public:
		FileCreateException(const string& message);
	};

	/// @brief An exception for errors related to file operations
	class FileOperationException :
		public Exception
	{
	public:
		FileOperationException(const string& message);
	};

	/// @brief A file that can read and write data
	class File : public InputTextStream, public OutputTextStream, public InputBinaryStream, public OutputBinaryStream
	{
	public:
		File(const FilePath& filePath, FileOpenFlags openFlags);

		~File();

		/// @brief Creates a file at the given path. Throws an error if the file already exists
		/// @param filePath The path for the file
		/// @param openFlags The flags to open the file with. FileOpenFlags::Write is included by default
		/// @return The created file
		static File Create(const FilePath& filePath, FileOpenFlags openFlags);

		/// @brief Checks if a file exists at the given path
		/// @param filePath The path to a file
		/// @return True if a file exists
		static bool Exists(const FilePath& filePath);

		/// @brief Reads a file as text
		/// @param filePath The path of the file
		/// @return The file as text
		static string ReadAllText(const FilePath& filePath);

		/// @brief Reads a file as binary data
		/// @param filePath The path of the file
		/// @return The file as binary data
		static std::vector<uint8> ReadAllBytes(const FilePath& filePath);

		uint64 GetPosition() const final { return _position; }
		void Seek(uint64 position, bool relative) final;
		uint8 Peek() final;
		uint64 Read(std::span<uint8> data) final;
		char PeekChar() final { return static_cast<char>(Peek()); }
		void PeekLine(string& outText, char lineTerminator) final;
		string ReadText(uint64 maxLength) final;
		bool ReadLine(string& outText, char lineTerminator) final;
		void Write(std::span<const uint8> data) final;
		void Flush() final;
		void Write(const string& text) final;
		void WriteLine(const string& text, char lineTerminator) final;

		/// @brief Reads from the current stream position to the end of the file as text
		/// @return The text
		string ReadTextToEnd();

		/// @brief Reads from the current stream position to the end of the file as bytes
		/// @return The data
		std::vector<uint8> ReadToEnd();

		/// @brief Gets the size of this file, in bytes
		/// @return The file size
		uint64 GetSize() const { return _size; }

		/// @brief Determines if this file is open
		/// @return True if this file is open
		bool IsOpen() const;

		/// @brief Closes this file
		void Close();

	private:
		FilePath _filePath;
		std::fstream _fileStream;
		uint64 _size;
		uint64 _position;
		FileOpenFlags _openFlags;

	private:
		/// @brief Synchronises this file's state with the underlying file state
		void SyncState();

		/// @brief Checks if this file is open and valid for the given flags
		/// @param flags The flags to check for
		void CheckFlags(FileOpenFlags flags);
	};
}