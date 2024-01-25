#include "Corepch.h"
#include "StringStream.h"

#include "../Engine.h"

namespace Coco
{
    StringStream::StringStream(const string& str) :
        _stream(str),
        _position(0)
    {}

    void StringStream::Seek(uint64 position, bool relative)
    {
        _stream.seekg(position, relative ? std::ios::cur : std::ios::beg);
        
        SyncState();
    }

    char StringStream::PeekChar()
    {
        return static_cast<char>(_stream.peek());
    }

    void StringStream::PeekLine(string& outText, char lineTerminator)
    {
        uint64 p = GetPosition();

        ReadLine(outText, lineTerminator);

        _stream.clear();
        Seek(p, false);

        CocoAssert(GetPosition() == p, "Failed to reset position after peek")
    }

    string StringStream::ReadText(uint64 maxLength)
    {
        if (maxLength == 0)
            return string();

        string text;
        text.resize(maxLength);

        _stream.read(text.data(), maxLength);
        SyncState();

        return text;
    }

    bool StringStream::ReadLine(string& outText, char lineTerminator)
    {
        uint64 p = GetPosition();
        std::getline(_stream, outText, lineTerminator);
        SyncState();

        return GetPosition() - p != 0;
    }

    void StringStream::Write(const string& text)
    {
        const char* d = reinterpret_cast<const char*>(text.data());

        _stream.write(d, text.size());
        SyncState();
    }

    void StringStream::WriteLine(const string& text, char lineTerminator)
    {
        Write(text + lineTerminator);
    }

    void StringStream::Flush()
    {
        _stream.flush();
    }

    string StringStream::ToString()
    {
        return _stream.str();
    }

    void StringStream::SyncState()
    {
        _position = _stream.tellg();
    }
}