#include "Stream.h"

#include <stdio.h>
#include <istream>

namespace Coco
{
	void StreamReader::Seek(uint64_t position, bool relative)
	{
		GetReadStream().seekg(position, relative ? std::ios::cur : std::ios::beg);
	}

	char StreamReader::Peek()
	{
		return GetReadStream().peek();
	}

	void StreamReader::PeekLine(string & text, char lineEnd)
	{
		std::istream& s = GetReadStream();
		const uint64_t pos = GetPosition();
		const std::ios_base::iostate state = s.rdstate();

		ReadLine(text, lineEnd);

		s.setstate(state);

		Seek(pos);
	}

	bool StreamReader::ReadLine(string& text, char lineEnd)
	{
		std::istream& s = GetReadStream();

		std::getline(s, text, lineEnd);

		return !text.empty();
	}

	List<char> StreamReader::Read(uint64_t bytesToRead)
	{
		List<char> bytes(bytesToRead);

		std::istream& s = GetReadStream();

		s.read(bytes.Data(), bytesToRead);
		const uint64_t bytesRead = s.gcount();

		if (bytesRead < bytesToRead)
			bytes.Resize(bytesRead);

		return bytes;
	}

	string StreamReader::ReadText(uint64_t charactersToRead)
	{
		List<char> chars = Read(charactersToRead);
		return string(chars.Data());
	}

	void StreamWriter::Write(const List<char>& bytes)
	{
		GetWriteStream().write(bytes.Data(), bytes.Count());
	}

	void StreamWriter::WriteText(const string& text)
	{
		GetWriteStream().write(text.c_str(), text.length());
	}

	void StreamWriter::WriteLine(const string& text, char lineEnd)
	{
		WriteText(text + lineEnd);
	}

	void StreamWriter::Flush()
	{
		GetWriteStream().flush();
	}
}