#include "KeyValueResourceSerializer.h"

namespace Coco
{
	KeyValueReader::KeyValueReader(std::istream& stream) : 
		_reader(stream), _currentIndentLevel(0), _currentLine(""), _currentKey(""), _currentValue("")
	{}

	uint64_t KeyValueReader::GetNextLineIndentLevel()
	{
		string nextLine;
		_reader.PeekLine(nextLine);
		return KeyValueResourceSerializer::GetIndentationLevel(nextLine);
	}

	bool KeyValueReader::ReadLine()
	{
		string line;
		const bool read = _reader.ReadLine(line);
		SetCurrentLine(line);

		return read;
	}

	bool KeyValueReader::ReadUntilIndentLevel(uint64_t indentLevel)
	{
		uint64_t nextIndent = 0;

		// Read until the next line is at most our target indent level
		do
		{
			nextIndent = GetNextLineIndentLevel();
		} while (nextIndent > indentLevel || ReadLine());

		return nextIndent == indentLevel;
	}

	bool KeyValueReader::ReadIfIsIndentLevel(uint64_t indentLevel)
	{
		return GetNextLineIndentLevel() == indentLevel && ReadLine();
	}

	void KeyValueReader::SetCurrentLine(const string& line)
	{
		_currentLine = line;

		KeyValueResourceSerializer::GetLinePair(_currentLine, _currentKey, _currentValue);
		_currentIndentLevel = KeyValueResourceSerializer::GetIndentationLevel(line);
	}


	/*KeyValueWriter::KeyValueWriter(std::ostream& stream, bool useTabs) :
		_stream(&stream), _useTabs(useTabs), _currentIndentLevel(0), _currentIndentStr("")
	{}

	void KeyValueWriter::SetIndentLevel(int indentLevel)
	{
		_currentIndentLevel = indentLevel;

		_currentIndentStr.clear();
		for (int i = 0; i < indentLevel; i++)
			_currentIndentStr += _useTabs ? "\t" : "    ";
	}

	void KeyValueWriter::WriteLine(const string& line)
	{
		_file->WriteLine(_currentIndentStr + line);
	}

	void KeyValueWriter::WriteLine(const string& key, const string& value)
	{
		WriteLine(FormattedString("{}={}", key, value));
	}*/

	uint64_t KeyValueResourceSerializer::GetIndentationLevel(const string& line) noexcept
	{
		if (line.starts_with('\t'))
		{
			return line.find_first_not_of('\t');
		}
		else
		{
			return line.find_first_not_of(' ') / 4;
		}
	}

	void KeyValueResourceSerializer::GetLinePair(const string& line, string& key, string& value)
	{
		if (line.empty())
		{
			key = "";
			value = "";
			return;
		}

		const size_t delimiter = line.find('=');

		if (delimiter == string::npos)
		{
			key = TrimWhitespace(line);
			value = "";
		}
		else
		{
			key = TrimWhitespace(line.substr(0, delimiter));
			value = TrimWhitespace(line.substr(delimiter + 1));
		}
	}
}