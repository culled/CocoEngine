#include "Stream.h"

#include <stdio.h>
#include <istream>

namespace Coco
{
	StreamReader::StreamReader(std::istream& stream) noexcept : _stream(&stream)
	{}

	uint64_t StreamReader::GetPosition()
	{ 
		return _stream->tellg(); 
	}

	void StreamReader::Seek(uint64_t position)
	{
		_stream->seekg(position);
	}

	void StreamReader::PeekLine(string & text, char lineEnd)
	{
		const uint64_t pos = GetPosition();
		const std::ios_base::iostate state = _stream->rdstate();

		ReadLine(text, lineEnd);

		_stream->setstate(state);

		Seek(pos);
	}

	bool StreamReader::ReadLine(string& text, char lineEnd)
	{
		if (!_stream->good())
			return false;

		std::getline(*_stream, text, lineEnd);

		return true;
	}
}