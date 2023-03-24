#include "KeyValueResourceLoader.h"

#include <Coco/Core/IO/File.h>

namespace Coco
{
	KeyValueReader::KeyValueReader(File& file) : 
		_file(&file), _currentIndentLevel(0), _currentLine(""), _currentKey(""), _currentValue("")
	{}

	int KeyValueReader::GetNextLineIndentLevel()
	{
		string nextLine;
		_file->PeekLine(nextLine);
		return KeyValueResourceLoader::GetIndentationLevel(nextLine);
	}

	bool KeyValueReader::ReadLine()
	{
		string line;
		bool read = _file->ReadLine(line);
		SetCurrentLine(line);

		return read;
	}

	bool KeyValueReader::ReadUntilIndentLevel(int indentLevel)
	{
		int nextIndent = 0;

		// Read until the next line is at most our target indent level
		do
		{
			nextIndent = GetNextLineIndentLevel();
		} while (nextIndent > indentLevel || ReadLine());

		return nextIndent == indentLevel;
	}

	bool KeyValueReader::ReadIfIsIndentLevel(int indentLevel)
	{
		return GetNextLineIndentLevel() == indentLevel && ReadLine();
	}

	void KeyValueReader::SetCurrentLine(const string& line)
	{
		_currentLine = line;

		KeyValueResourceLoader::GetLinePair(_currentLine, _currentKey, _currentValue);
		_currentIndentLevel = KeyValueResourceLoader::GetIndentationLevel(line);
	}


	KeyValueWriter::KeyValueWriter(File& file, bool useTabs) : 
		_file(&file), _useTabs(useTabs), _currentIndentLevel(0), _currentIndentStr("")
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
	}

	KeyValueResourceLoader::KeyValueResourceLoader(ResourceLibrary* library) : ResourceLoader(library)
	{}

	int KeyValueResourceLoader::GetIndentationLevel(const string& line)
	{
		if (line.starts_with('\t'))
		{
			return static_cast<int>(line.find_first_not_of('\t'));
		}
		else
		{
			const size_t lastIndentIndex = line.find_first_not_of(' ');
			return static_cast<int>(lastIndentIndex) / 4;
		}
	}

	void KeyValueResourceLoader::GetLinePair(const string& line, string& key, string& value)
	{
		if (line.empty())
		{
			key = "";
			value = "";
			return;
		}

		size_t delimiter = line.find('=');

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