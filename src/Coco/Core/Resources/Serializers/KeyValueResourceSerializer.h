#pragma once

#include "../IResourceSerializer.h"
#include <Coco/Core/IO/Stream.h>

namespace Coco
{
	class File;

	/// @brief A reader that can read lines in a file as key=value pairs
	class COCOAPI KeyValueReader
	{
	private:
		StreamReader _reader;
		string _currentLine;
		string _currentKey;
		string _currentValue;
		uint64_t _currentIndentLevel;

	public:
		KeyValueReader(std::istream& stream);
		virtual ~KeyValueReader() = default;

		/// @brief Gets the current line
		/// @return The current line
		const string& GetCurrentLine() const noexcept { return _currentLine; }

		/// @brief Gets the current key
		/// @return The current key
		const string& GetKey() const noexcept { return _currentKey; }

		/// @brief Determines if the current line has a key with the given name
		/// @param key The key
		/// @return True if the current line has a key matching the given name
		bool IsKey(const string& key) const noexcept { return _currentKey == key; }

		/// @brief Gets the current key as an integer
		/// @return The current key as an integer
		int GetKeyAsInt() const noexcept { return atoi(_currentKey.c_str()); }

		/// @brief Gets the current key as a boolean
		/// @return The current key as a boolean
		bool GetKeyAsBool() const noexcept { return atoi(_currentKey.c_str()) == 1; }

		/// @brief Gets the current value
		/// @return The current value, or an empty string if the line has no value
		const string& GetValue() const noexcept { return _currentValue; }

		/// @brief Gets the current value as an integer
		/// @return The current value as an integer
		int GetVariableValueAsInt() const noexcept { return atoi(_currentValue.c_str()); }

		/// @brief Gets the current value as a boolean
		/// @return The current value as a boolean
		bool GetVariableValueAsBool() const noexcept { return atoi(_currentValue.c_str()) == 1; }

		/// @brief Gets the indent level of the current line
		/// @return The indent level of the current line
		constexpr uint64_t GetCurrentIndentLevel() const noexcept { return _currentIndentLevel; }

		/// @brief Gets the indent level of the next line
		/// @return The indent level of the next line 
		uint64_t GetNextLineIndentLevel();

		/// @brief Reads the next line
		/// @return True if a line was read
		bool ReadLine();

		/// @brief Reads until the next line is at the given indent level or less
		/// @param indentLevel The desired indent level
		/// @return True if the next line is at the desired indent level
		bool ReadUntilIndentLevel(uint64_t indentLevel);

		/// @brief Reads the next line only if it is the given indent level
		/// @param indentLevel The indent level
		/// @return True if the line was read
		bool ReadIfIsIndentLevel(uint64_t indentLevel);

	private:
		/// @brief Sets the current line information
		/// @param line The current line
		void SetCurrentLine(const string& line);
	};

	/// @brief A writer that can write key=value pair lines to a file
	/*class COCOAPI KeyValueWriter
	{
	private:
		std::ostream* _stream;
		int _currentIndentLevel = 0;
		string _currentIndentStr = "";
		bool _useTabs;

	public:
		KeyValueWriter(std::ostream& stream, bool useTabs = false);
		virtual ~KeyValueWriter() = default;

		/// @brief Sets the indent level for subsequent line writes
		/// @param indentLevel The indent level for subsequent lines
		void SetIndentLevel(int indentLevel);

		/// @brief Increments the indent level for subsequent line writes
		void IncrementIndentLevel() { SetIndentLevel(_currentIndentLevel + 1); }

		/// @brief Decrements the indent level for subsequent line writes
		void DecrementIndentLevel() { SetIndentLevel(Math::Max(_currentIndentLevel - 1, 0)); }

		/// @brief Writes a line to the file
		/// @param line The line to write
		void WriteLine(const string& line);

		/// @brief Writes a key=value pair to the file
		/// @param key The key
		/// @param value The value
		void WriteLine(const string& key, const string& value);
	};*/

	/// @brief A serializer that can serialize/deserialize files in a key=value pair format
	class COCOAPI KeyValueResourceSerializer : public IResourceSerializer
	{
		friend KeyValueReader;

	protected:
		KeyValueResourceSerializer() = default;

	public:
		virtual ~KeyValueResourceSerializer() = default;

	protected:
		/// @brief Gets the indentation level of a string (supports tabs and 4-space tabs)
		/// @param line The line
		/// @return The indentation level
		static uint64_t GetIndentationLevel(const string& line) noexcept;

		/// @brief Gets a key=value pair from a line
		/// @param line The line
		/// @param key Will be filled with the key
		/// @param value Will be filled with the value, or an empty string if the line was not a key=value pair
		static void GetLinePair(const string& line, string& key, string& value);
	};
}