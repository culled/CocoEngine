#pragma once

#include "../ResourceLoader.h"

namespace Coco
{
	class File;

	/// @brief A reader that can read lines in a file as key=value pairs
	class COCOAPI KeyValueReader
	{
	private:
		File* _file;
		string _currentLine;
		string _currentKey;
		string _currentValue;
		int _currentIndentLevel;

	public:
		KeyValueReader(File& file);
		virtual ~KeyValueReader() = default;

		/// @brief Gets the current line
		/// @return The current line
		const string& GetCurrentLine() const { return _currentLine; }

		/// @brief Gets the current key
		/// @return The current key
		const string& GetKey() const { return _currentKey; }

		/// @brief Determines if the current line has a key with the given name
		/// @param key The key
		/// @return True if the current line has a key matching the given name
		bool IsKey(const string& key) const { return _currentKey == key; }

		/// @brief Gets the current key as an integer
		/// @return The current key as an integer
		int GetKeyAsInt() const { return atoi(_currentKey.c_str()); }

		/// @brief Gets the current key as a boolean
		/// @return The current key as a boolean
		bool GetKeyAsBool() const { return atoi(_currentKey.c_str()) == 1; }

		/// @brief Gets the current value
		/// @return The current value, or an empty string if the line has no value
		const string& GetValue() const { return _currentValue; }

		/// @brief Gets the current value as an integer
		/// @return The current value as an integer
		int GetVariableValueAsInt() const { return atoi(_currentValue.c_str()); }

		/// @brief Gets the current value as a boolean
		/// @return The current value as a boolean
		bool GetVariableValueAsBool() const { return atoi(_currentValue.c_str()) == 1; }

		/// @brief Gets the indent level of the current line
		/// @return The indent level of the current line
		constexpr int GetCurrentIndentLevel() const { return _currentIndentLevel; }

		/// @brief Gets the indent level of the next line
		/// @return The indent level of the next line 
		int GetNextLineIndentLevel();

		/// @brief Reads the next line
		/// @return True if a line was read
		bool ReadLine();

		/// @brief Reads until the next line is at the given indent level or less
		/// @param indentLevel The desired indent level
		/// @return True if the next line is at the desired indent level
		bool ReadUntilIndentLevel(int indentLevel);

		/// @brief Reads the next line only if it is the given indent level
		/// @param indentLevel The indent level
		/// @return True if the line was read
		bool ReadIfIsIndentLevel(int indentLevel);

	private:
		/// @brief Sets the current line information
		/// @param line The current line
		void SetCurrentLine(const string& line);
	};

	/// @brief A writer that can write key=value pair lines to a file
	class COCOAPI KeyValueWriter
	{
	private:
		File* _file;
		int _currentIndentLevel = 0;
		string _currentIndentStr = "";
		bool _useTabs;

	public:
		KeyValueWriter(File& file, bool useTabs = false);
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
	};

	/// @brief A loader that can save/load files in a key=value pair format
	class COCOAPI KeyValueResourceLoader : public ResourceLoader
	{
		friend KeyValueReader;

	protected:
		KeyValueResourceLoader(ResourceLibrary* library);

	public:
		virtual ~KeyValueResourceLoader() = default;

	protected:
		/// @brief Gets the indentation level of a string (supports tabs and 4-space tabs)
		/// @param line The line
		/// @return The indentation level
		static int GetIndentationLevel(const string& line);

		/// @brief Gets a key=value pair from a line
		/// @param line The line
		/// @param key Will be filled with the key
		/// @param value Will be filled with the value, or an empty string if the line was not a key=value pair
		static void GetLinePair(const string& line, string& key, string& value);
	};
}