#include "File.h"

#include <filesystem>
#include <stdio.h>
#include <fstream>

namespace Coco
{
	File::File(const string& path, FileModes openModes)
	{
		int mode = 0;

		if (openModes == FileModes::None)
			throw Exception("Invalid open mode");

		if (openModes & FileModes::Read)
			mode |= std::ios::in;

		if (openModes & FileModes::Write)
			mode |= std::ios::out;

		if (openModes & FileModes::Binary)
			mode |= std::ios::binary;

		_handle.open(path, mode);

		if (!_handle.is_open())
		{
			string err = FormattedString("Failed to open file {}", path);
			throw Exception(err.c_str());
		}

		// By default, have all write operations start at the end of the file
		_handle.seekp(0, std::ios::end);

		_path = path;
	}

	File::~File()
	{
		if (_handle.is_open())
			Close();
	}

	bool File::Exists(const string& path)
	{
		return std::filesystem::exists(path);
	}

	string File::ReadAllText(const string& path)
	{
		File f = Open(path, FileModes::Read);
		string text = f.ReadTextToEnd();
		f.Close();
		return text;
	}

	List<uint8_t> File::ReadAllBytes(const string& path)
	{
		File f = Open(path, FileModes::Read);
		List<uint8_t> bytes = f.ReadToEnd();
		f.Close();
		return bytes;
	}

	uint8_t File::Peek()
	{
		CheckHandle();

		return _handle.peek();
	}

	List<uint8_t> File::Read(uint64_t bytesToRead)
	{
		CheckHandle();

		List<uint8_t> bytes(bytesToRead);

		_handle.read(reinterpret_cast<char*>(bytes.Data()), bytesToRead);
		uint64_t bytesRead = _handle.gcount();

		if (bytesRead < bytesToRead)
			bytes.Resize(bytesRead);

		return bytes;
	}

	List<uint8_t> File::ReadToEnd()
	{
		CheckHandle();

		std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);

		uint64_t length = _handle.tellg() - current;

		_handle.seekg(current);

		return Read(length);
	}

	void File::Write(const List<uint8_t>& bytes)
	{
		CheckHandle();

		_handle.write(reinterpret_cast<const char*>(bytes.Data()), bytes.Count());
	}

	string File::ReadText(uint64_t charactersToRead)
	{
		CheckHandle();

		List<char> chars(charactersToRead);
		_handle.get(chars.Data(), charactersToRead);

		uint64_t charactersRead = _handle.gcount();
		if (charactersRead < charactersToRead)
			chars.Resize(charactersRead);

		return string(chars);
	}

	string File::ReadTextToEnd()
	{
		CheckHandle();

		std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::end);

		uint64_t length = _handle.tellg() - current;

		_handle.seekg(current);

		return ReadText(length);
	}

	void File::WriteText(const string& text)
	{
		CheckHandle();

		_handle.write(text.c_str(), text.length());
	}

	bool File::ReadLine(string& text, char lineEnd)
	{
		CheckHandle();

		std::getline(_handle, text, lineEnd);

		return !_handle.eof();
	}

	void File::WriteLine(const string& text)
	{
		string writeText = text + '\n';

		WriteText(writeText);
	}

	void File::SeekWrite(uint64_t position)
	{
		CheckHandle();

		_handle.seekp(position);
	}

	void File::SeekWriteRelative(int64_t positionOffset)
	{
		CheckHandle();

		_handle.seekp(positionOffset, std::ios::cur);
	}

	uint64_t File::GetWritePosition()
	{
		CheckHandle();

		return _handle.tellp();
	}

	void File::SeekRead(uint64_t position)
	{
		CheckHandle();

		_handle.seekg(position);
	}

	void File::SeekReadRelative(int64_t positionOffset)
	{
		CheckHandle();

		_handle.seekg(positionOffset, std::ios::cur);
	}

	uint64_t File::GetReadPosition()
	{
		CheckHandle();

		return _handle.tellg();
	}

	uint64_t File::GetSize()
	{
		CheckHandle();

		std::streampos current = _handle.tellg();

		_handle.seekg(0, std::ios::beg);
		std::streampos start = _handle.tellg();

		_handle.seekg(0, std::ios::end);
		std::streampos end = _handle.tellg();

		_handle.seekg(current);

		return end - start;
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
		_path = string();
	}

	void File::CheckHandle() const
	{
		if (!_handle.is_open())
			throw Exception("File is not opened");
	}
}