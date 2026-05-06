//
// Created by cullen on 3/10/26.
//

#include "File.h"

#include <filesystem>
#include <utility>

#include "Coco/Core/Types/TimeSpan.h"

namespace Coco
{
    File::File() noexcept :
        _path(),
        _stream(),
        _openFlags(FileOpenFlags::None),
        _size(0),
        _position(0)
    {}

    File::File(FilePath path, FileOpenFlags openFlags) :
        _path(std::move(path)),
        _stream(),
        _openFlags(openFlags),
        _size(0),
        _position(0)
    {
        std::ios_base::openmode mode = std::ios_base::binary;

        if ((_openFlags & FileOpenFlags::Write) == FileOpenFlags::Write)
            mode |= std::ios::out;

        if (mode == 0 || (_openFlags & FileOpenFlags::Read) == FileOpenFlags::Read)
            mode |= std::ios::in;

        // Start at the end so we can determine the file's size
        mode |= std::ios::ate;

        _stream = std::fstream(_path.CStr(), mode);

        if (!_stream.is_open())
            throw FileOpenException(_path);

        // Record the file size
        _size = _stream.tellg();
        _stream.seekg(0, std::ios::beg);
    }

    File File::Create(FilePath filePath, FileOpenFlags openFlags, bool overwrite)
    {
        if (Exists(filePath) && !overwrite)
            throw FileCreateException(FormatString("File already exists at \"%s\"", filePath.CStr()));

        return {std::move(filePath), openFlags};
    }

    bool File::Exists(const FilePath& path)
    {
        return std::filesystem::exists(path.CStr());
    }

    void File::ReadAllBytes(FilePath filePath, ArrayContainer<uint8>& outData)
    {
        File f(std::move(filePath), FileOpenFlags::Read);
        f.ReadToEnd(outData);
        f.Close();
    }

    void File::ReadAllText(FilePath filePath, String& outText)
    {
        File f(std::move(filePath), FileOpenFlags::Read);
        f.ReadTextToEnd(outText);
        f.Close();
    }

    void File::Seek(uint64 position, bool absolute)
    {
        CheckOperation(FileOpenFlags::None);

        _stream.seekg(static_cast<uint32>(position), absolute ? std::ios_base::beg : std::ios_base::cur);

        SyncState();
    }

    void File::Flush()
    {
        CheckOperation(FileOpenFlags::Write);

        _stream.flush();
    }

    void File::Close()
    {
        if (!IsOpen())
            return;

        if ((_openFlags & FileOpenFlags::Write) == FileOpenFlags::Write)
            Flush();

        _stream.close();
    }

    uint8 File::Peek()
    {
        CheckOperation(FileOpenFlags::None);

        return static_cast<uint8>(_stream.peek());
    }

    uint64 File::Read(Span<uint8> outData)
    {
        CheckOperation(FileOpenFlags::Read);

        if (outData.empty())
            return 0;

        char* d = reinterpret_cast<char*>(outData.data());

        _stream.read(d, static_cast<uint32>(outData.size()));
        SyncState();

        return _stream.gcount();
    }

    uint64 File::ReadToEnd(ArrayContainer<uint8>& outData)
    {
        CheckOperation(FileOpenFlags::Read);

        uint64 bytesLeft = GetSize() - GetPosition();
        outData.Resize(bytesLeft);

        return Read(Span<uint8>(outData.Data(), bytesLeft));
    }

    void File::Write(uint8 byte)
    {
        CheckOperation(FileOpenFlags::Write);

        const char* d = reinterpret_cast<const char*>(&byte);

        _stream.write(d, 1);

        SyncState();
    }

    void File::Write(Span<const uint8> data)
    {
        CheckOperation(FileOpenFlags::Write);

        _stream.write(reinterpret_cast<const char*>(data.data()), static_cast<uint32>(data.size()));

        SyncState();
    }

    char File::PeekChar()
    {
        return static_cast<char>(Peek());
    }

    uint64 File::ReadText(uint64 maxCharacters, String& outText)
    {
        CheckOperation(FileOpenFlags::Read);

        if (maxCharacters == 0)
            return 0;

        outText.Reserve(maxCharacters);
        _stream.read(outText.Data(), static_cast<uint32>(maxCharacters));

        SyncState();

        return _stream.gcount();
    }

    bool File::ReadLine(String& outText, char lineTerminator)
    {
        constexpr uint32 bufferSize = 1024;
        char buffer[bufferSize];
        uint32 totalRead = 0;

        while (_stream.getline(buffer, bufferSize, lineTerminator))
        {
            uint32 read = _stream.gcount();
            totalRead += read;
            outText.Append(buffer, read);
        }

        return totalRead > 0;
    }

    uint64 File::ReadTextToEnd(String& outText)
    {
        CheckOperation(FileOpenFlags::Read);

        uint64 bytesLeft = GetSize() - GetPosition();
        outText.Resize(bytesLeft, false);

        return Read(Span<uint8>(reinterpret_cast<uint8*>(outText.Data()), bytesLeft));
    }

    void File::WriteChar(char character)
    {
        Write(static_cast<uint8>(character));
    }

    void File::WriteText(const char* text)
    {
        uint64 len = strlen(text);
        Write(Span<const uint8>(reinterpret_cast<const uint8*>(text), len));
    }

    void File::WriteText(const String& text)
    {
        Write(Span<const uint8>(reinterpret_cast<const uint8*>(text.Data()), text.GetLength()));
    }

    void File::WriteLine(const char* text, char lineTerminator)
    {
        WriteText(text);
        WriteChar(lineTerminator);
    }

    void File::WriteLine(const String& text, char lineTerminator)
    {
        WriteText(text);
        WriteChar(lineTerminator);
    }

    void File::CheckOperation(FileOpenFlags requiredFlags) const
    {
        if ((requiredFlags & _openFlags) != requiredFlags)
        {
            throw FileOperationException(
                FormatString(
                    "File was not opened with required flags for the operation. File flags: %d, required flags: %d",
                    static_cast<int>(_openFlags),
                    static_cast<int>(requiredFlags)
                )
            );
        }

        if (!_stream.is_open())
            throw FileOperationException("File stream was not open");
    }

    void File::SyncState()
    {
        _position = _stream.tellg();
        _size = Math::Max(_position, _size);
    }
} // Coco