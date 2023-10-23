#pragma once

namespace Coco
{
	/// @brief Severity of log messages
	enum class LogMessageSeverity
	{
		Trace,
		Info,
		Warning,
		Error,
		Critical
	};

	/// @brief Gets the string representation of a LogMessageSeverity value
	/// @param severity The severity
	/// @return The string representation of the severity
	constexpr const char* GetLogMessageSeverityString(LogMessageSeverity severity)
	{
		switch (severity)
		{
		case LogMessageSeverity::Trace:
			return "Trace";
		case LogMessageSeverity::Info:
			return "Info";
		case LogMessageSeverity::Warning:
			return "Warning";
		case LogMessageSeverity::Error:
			return "Error";
		case LogMessageSeverity::Critical:
			return "Critical";
		default:
			return "Unknown";
		}
	}
}
