#include "File.h"
#include "FileExceptions.h"

#include <filesystem>
#include <stdio.h>
#include <fstream>

namespace Coco
{
	File::File(const string& path, FileModeFlags openFlags) :
		_openFlags(FileModeFlags::None),
		Path(path)
	{
		int mode = 0;

		if (openFlags == FileModeFlags::None)
			throw FileOpenException("Invalid open mode");

		if ((openFlags & FileModeFlags::Read) == FileModeFlags::Read)
			mode |= std::ios::in;

		if ((openFlags & FileModeFlags::Write) == FileModeFlags::Write)
			mode |= std::ios::out;

		if ((openFlags & FileModeFlags::Binary) == FileModeFlags::Binary)
			mode |= std::ios::binary;

		_handle.open(path, mode);

		if (!_handle.is_open())
			throw FileOpenException(FormattedString("Failed to open file  \"{}\"", path));

		// By default, have all operations start at the beginning of the file
		_handle.seekg(0, std::ios::beg);

		_openFlags = openFlags;
	}

	File::File(File&& other) noexcept :
		_openFlags(other._openFlags), 
		Path(other.Path), 
		_handle(std::move(other._handle))
	{}

	File::~File()
	{
		try
		{
			if (_handle.is_open())
				Close();
		}
		catch(...)
		{ }
	}

	bool File::Exists(const string& path)
	{
		return std::filesystem::exists(path);
	}

	string File::ReadAllText(const string& path)
	{
		File f = Open(path, FileModeFlags::Read);
		string text = f.ReadTextToEnd();
		f.Close();
		return text;
	}

	List<char> File::ReadAllBytes(const string& path)
	{
		File f = Open(path, FileModeFlags::Read | FileModeFlags::Binary);
		List<char> bytes = f.ReadToEnd();
		f.Close();
		return bytes;
	}

	void File::WriteAllText(const string& path, const string& text)
	{
		File f = Open(path, FileModeFlags::Write);
		f.WriteText(text);
		f.Close();
	}

	void File::WriteAllBytes(const string& path, const List<char>& data)
	{
		File f = Open(path, FileModeFlags::Write | FileModeFlags::Binary);
		f.Write(data);
		f.Close();
	}

	char File::Peek()
	{
		CheckHandle();

		return _handle.peek();
	}

	List<char> File::Read(uint64_t bytesToRead)
	{
		CheckHandle();

		List<char> bytes(bytesToRead);

		_handle.read(bytes.Data(), bytesToRead);
		const uint64_t bytesRead = _handle.gcount();

		if (bytesRead < bytesToRead)
			bytes.Resize(bytesRead);

		return bytes;
	}

	List<char> File::ReadToEnd()
	{
		CheckHandle();

		const std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);

		const uint64_t length = _handle.tellg() - current;

		_handle.seekg(current);

		return Read(length);
	}

	void File::Write(const List<char>& bytes)
	{
		CheckHandle();

		_handle.write(bytes.Data(), bytes.Count());
	}

	string File::ReadText(uint64_t charactersToRead)
	{
		CheckHandle();

		List<char> chars(charactersToRead);
		_handle.read(chars.Data(), charactersToRead);

		const uint64_t charactersRead = _handle.gcount();
		if (charactersRead < charactersToRead)
			chars.Resize(charactersRead);

		return string(chars.Data());
	}

	string File::ReadTextToEnd()
	{
		CheckHandle();

		const std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);

		const uint64_t length = _handle.tellg() - current;

		_handle.seekg(current);

		return ReadText(length);
	}

	void File::WriteText(const string& text)
	{
		CheckHandle();

		_handle.write(text.c_str(), text.length());
	}

	void File::WriteLine(const string& text, char lineEnd)
	{
		WriteText(text + lineEnd);
	}

	bool File::ReadLine(string& text, char lineEnd)
	{
		CheckHandle();

		if (!_handle.good())
			return false;

		std::getline(_handle, text, lineEnd);

		return true;
	}

	void File::PeekLine(string& text, char lineEnd)
	{
		CheckHandle();

		const uint64_t pos = GetPosition();
		const std::ios_base::iostate state = _handle.rdstate();

		ReadLine(text, lineEnd);

		_handle.setstate(state);

		Seek(pos);
	}

	void File::Seek(uint64_t position)
	{
		CheckHandle();

		_handle.seekg(position);
	}

	void File::SeekRelative(int64_t positionOffset)
	{
		CheckHandle();

		_handle.seekg(positionOffset, std::ios::cur);
	}

	uint64_t File::GetPosition()
	{
		CheckHandle();

		return _handle.tellg();
	}

	uint64_t File::GetSize()
	{
		CheckHandle();

		const std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);
		const uint64_t length = _handle.tellg();

		_handle.seekg(current);

		return length;
	}

	void File::Flush()
	{
		CheckHandle();

		_handle.flush();
	}

	void File::Close()
	{
		if (!_handle.is_open())
			return;

		Flush();

		_handle.close();
	}

	void File::CheckHandle() const
	{
		if (!_handle.is_open())
			throw FileException("File operations require an opened file");
	}
}