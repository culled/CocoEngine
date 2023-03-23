#pragma once

#include "../ResourceLoader.h"

namespace Coco
{
	class File;

	/// <summary>
	/// A reader that can read lines in a file as key value pairs
	/// </summary>
	class COCOAPI KeyValueReader
	{
	private:
		File* _file;
		string _currentLine;
		string _currentVariable;
		string _currentVariableValue;
		int _currentIndentLevel;

	public:
		KeyValueReader(File& file);
		virtual ~KeyValueReader() = default;

		/// <summary>
		/// Gets the current line without any whitespace
		/// </summary>
		/// <returns>The current line</returns>
		const string& GetCurrentLine() const { return _currentLine; }

		/// <summary>
		/// Gets the current variable
		/// </summary>
		/// <returns>The current variable</returns>
		const string& GetVariable() const { return _currentVariable; }

		/// <summary>
		/// Determines if the current line has a variable with the given name
		/// </summary>
		/// <param name="variableName">The name of the variable</param>
		/// <returns>True if the current line has a variable matching the given name</returns>
		bool IsVariable(const string& variableName) const { return _currentVariable == variableName; }

		/// <summary>
		/// Gets the current variable as an integer
		/// </summary>
		/// <returns>The variable as an integer</returns>
		const int GetVariableAsInt() const { return atoi(_currentVariable.c_str()); }

		/// <summary>
		/// Gets the current variable as a boolean
		/// </summary>
		/// <returns>The variable as a boolean</returns>
		const bool GetVariableAsBool() const { return atoi(_currentVariable.c_str()) == 1; }

		/// <summary>
		/// Gets the current variable value
		/// </summary>
		/// <returns>The current variable value, or an empty string if the line has no variable value</returns>
		const string& GetVariableValue() const { return _currentVariableValue; }

		/// <summary>
		/// Gets the current variable value as an integer
		/// </summary>
		/// <returns>The variable value as an integer</returns>
		const int GetVariableValueAsInt() const { return atoi(_currentVariableValue.c_str()); }

		/// <summary>
		/// Gets the current variable value as a boolean
		/// </summary>
		/// <returns>The variable value as a boolean</returns>
		const bool GetVariableValueAsBool() const { return atoi(_currentVariableValue.c_str()) == 1; }

		/// <summary>
		/// Gets the indent level of the current line
		/// </summary>
		/// <returns>The indent level of the current line</returns>
		const int GetCurrentIndentLevel() const { return _currentIndentLevel; }

		/// <summary>
		/// Gets the indent level of the next line
		/// </summary>
		/// <returns>The indent level of the next line</returns>
		int GetNextLineIndentLevel();

		/// <summary>
		/// Reads the next line
		/// </summary>
		/// <returns>True if a line was read</returns>
		bool ReadLine();

		/// <summary>
		/// Reads until the next line is at the given indent level or less
		/// </summary>
		/// <param name="indentLevel">The desired indent level</param>
		/// <returns>True if the next line is at the desired indent level</returns>
		bool ReadUntilIndentLevel(int indentLevel);

		/// <summary>
		/// Reads the next line only if it is the given indent level
		/// </summary>
		/// <param name="indentLevel">The indent level</param>
		/// <returns>True if the line was read</returns>
		bool ReadIfIsIndentLevel(int indentLevel);

	private:
		/// <summary>
		/// Sets the current line information
		/// </summary>
		/// <param name="line">The current line</param>
		void SetCurrentLine(const string& line);
	};

	/// <summary>
	/// A writer that can write key value pair lines to a file
	/// </summary>
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

		/// <summary>
		/// Sets the indent level for subsequent line writes
		/// </summary>
		/// <param name="indentLevel">The indent level for subsequent lines</param>
		void SetIndentLevel(int indentLevel);

		/// <summary>
		/// Increments the indent level for subsequent line writes
		/// </summary>
		void IncrementIndentLevel() { SetIndentLevel(_currentIndentLevel + 1); }

		/// <summary>
		/// Decrements the indent level for subsequent line writes
		/// </summary>
		void DecrementIndentLevel() { SetIndentLevel(Math::Max(_currentIndentLevel - 1, 0)); }

		/// <summary>
		/// Writes a line to the file
		/// </summary>
		/// <param name="line">The line to write</param>
		void WriteLine(const string& line);

		/// <summary>
		/// Writes a key=value pair to the file
		/// </summary>
		/// <param name="variable">The key</param>
		/// <param name="value">The value</param>
		void WriteLine(const string& variable, const string& value);
	};

	/// <summary>
	/// A loader that can save/load files in a key value pair format
	/// </summary>
	class COCOAPI KeyValueResourceLoader : public ResourceLoader
	{
		friend KeyValueReader;

	protected:
		KeyValueResourceLoader(ResourceLibrary* library, const string& basePath);

	public:
		virtual ~KeyValueResourceLoader() = default;

	protected:
		/// <summary>
		/// Gets the indentation level of a string (supports tabs and 4-space tabs)
		/// </summary>
		/// <param name="line">The line</param>
		/// <returns>The indentation level</returns>
		static int GetIndentationLevel(const string& line);

		/// <summary>
		/// Gets a key value pair from a line
		/// </summary>
		/// <param name="line">The line</param>
		/// <param name="variable">The key</param>
		/// <param name="value">The value, or an empty string if the line was not a key-value pair</param>
		static void GetLinePair(const string& line, string& variable, string& value);
	};
}