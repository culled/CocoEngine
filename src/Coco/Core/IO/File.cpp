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

	List<char> File::ReadToEnd()
	{
		CheckHandle();

		const std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);

		const uint64_t length = _handle.tellg() - current;

		_handle.seekg(current);

		return Read(length);
	}

	string File::ReadTextToEnd()
	{
		List<char> data = ReadToEnd();
		return string(data.Data(), data.Count());
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

	void File::Close()
	{
		if (!_handle.is_open())
			return;

		Flush();

		_handle.close();
	}

	std::istream& File::GetReadStream()
	{
		CheckHandle();

		return _handle;
	}

	std::ostream& File::GetWriteStream()
	{
		CheckHandle();

		return _handle;
	}

	void File::CheckHandle() const
	{
		if (!_handle.is_open())
			throw FileException("File operations require an opened file");
	}
}