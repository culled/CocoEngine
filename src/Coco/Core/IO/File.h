//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_FILE_H
#define COCOENGINE_FILE_H
#include "FilePath.h"
#include "FileTypes.h"

#include <fstream>

#include "Coco/Core/Types/ArrayContainer.h"
#include "Coco/Core/Types/Span.h"

namespace Coco
{
    /// @brief A class that can be used to read and write files to disk
    class File
    {
    public:
        File() noexcept;
        File(FilePath  path, FileOpenFlags openFlags);

        /// @brief Creates a file at the given path
        /// @param filePath The path of the file to create
        /// @param openFlags Flags to open the created file with
        /// @param overwrite If true, any file that already exists will be overwritten. If false and a file already exists, a FileCreateException will be thrown
        /// @return The created file
        static File Create(FilePath filePath, FileOpenFlags openFlags, bool overwrite = false);

        /// @brief Determines if a file exists at the given path
        /// @param path The path of the file
        /// @return True if a file exists at the given path
        static bool Exists(const FilePath& path);

        /// @brief Reads the contents of a file as bytes
        /// @param filePath The path of the file
        /// @param outData Will be filled with the contents of the file
        static void ReadAllBytes(FilePath filePath, ArrayContainer<uint8>& outData);

        /// @brief Reads the contents of a file as text
        /// @param filePath The path of the file
        /// @param outText Will be filled with the contents of the file
        static void ReadAllText(FilePath filePath, String& outText);

        /// @brief Gets the current position in the file
        /// @return The position (in bytes)
        uint64 GetPosition() const noexcept { return _position; }

        /// @brief Gets the size of the file
        /// @return The size of the file (in bytes)
        uint64 GetSize() const noexcept { return _size; }

        /// @brief Determines if the position is at the end of the file
        /// @return True if the position is at the end of the file
        bool IsAtEnd() const noexcept { return _stream.eof(); }

        /// @brief Determines if the file is open
        /// @return True if the file is open
        bool IsOpen() const noexcept { return _size > 0 ? _stream.is_open() : false; }

        /// @brief Seeks to a position in the file
        /// @param position The new position (in bytes)
        /// @param absolute If true, the position will be relative to the start of the file, else it will be relative to its current position
        void Seek(uint64 position, bool absolute);

        /// @brief Causes any buffered data to be written
        void Flush();

        /// @brief Closes the file
        void Close();

        /// @brief Returns the byte at the current position without advancing the position
        /// @return The byte at the current position
        uint8 Peek();

        /// @brief Reads up to a number of bytes and copies them to a Span
        /// @param outData The Span to append the data to. Reading will stop when the Span has been filled
        /// @return The actual number of bytes read
        uint64 Read(Span<uint8> outData);

        /// @brief Reads the remaining bytes in the file
        /// @param outData The container to append the data to
        /// @return The number of bytes read
        uint64 ReadToEnd(ArrayContainer<uint8>& outData);

        /// @brief Writes a byte to the file
        /// @param byte The byte to write
        void Write(uint8 byte);

        /// @brief Writes a number of bytes to the file
        /// @param data The bytes to write
        void Write(Span<const uint8> data);

        /// @brief Returns the character at the current position without advancing the position
        /// @return The character at the current position
        char PeekChar();

        /// @brief Reads up to a number of characters into a String
        /// @param maxCharacters The maximum number of characters to read
        /// @param outText The String to append the data to
        /// @return The actual number of characters read
        uint64 ReadText(uint64 maxCharacters, String& outText);

        /// @brief Attempts to read a line of text into a String
        /// @param outText The String to append the data to
        /// @param lineTerminator The line terminator character
        /// @return True if a line was read
        bool ReadLine(String& outText, char lineTerminator = '\n');

        /// @brief Reads the remaining characters in the file
        /// @param outText The String to append the data to
        /// @return The number of characters read
        uint64 ReadTextToEnd(String& outText);

        /// @brief Writes a character to the file
        /// @param character The character to write
        void WriteChar(char character);

        /// @brief Writes null-terminated text to the file
        /// @param text The null-terminated text to write
        void WriteText(const char* text);

        /// @brief Writes a String to the file
        /// @param text The String to write
        void WriteText(const String& text);

        /// @brief Writes a line of null-terminated text to the file
        /// @param text The text to write
        /// @param lineTerminator The line terminator to add at the end of the text
        void WriteLine(const char* text, char lineTerminator = '\n');

        /// @brief Writes a line of null-terminated text to the file
        /// @param text The String to write
        /// @param lineTerminator The line terminator to add at the end of the text
        void WriteLine(const String& text, char lineTerminator = '\n');

    private:
        FilePath _path;
        std::fstream _stream;
        FileOpenFlags _openFlags;
        uint64 _size;
        uint64 _position;

        /// @brief Checks if the file had been opened with the required flags
        /// @param requiredFlags The required operation flags
        void CheckOperation(FileOpenFlags requiredFlags) const;

        /// @brief Synchronizes the state with the file state
        void SyncState();
    };
} // Coco

#endif //COCOENGINE_FILE_H