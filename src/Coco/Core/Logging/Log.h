#pragma once

#include "../Corepch.h"
#include "../Types/String.h"
#include "../Types/Refs.h"
#include "LogTypes.h"
#include "LogSink.h"

namespace Coco
{
	/// @brief A log that messages can be written to
	class Log
	{
	private:
		string _name;
		std::vector<SharedRef<LogSink>> _sinks;
		LogMessageSeverity _minSeverity;

	public:
		Log(const string& name, LogMessageSeverity minimumSeverity);
		~Log();

		/// @brief Adds a sink to this log
		/// @param sink The sink
		void AddSink(const SharedRef<LogSink>& sink);

		/// @brief Removes a sink from this log
		/// @param sink The sink
		void RemoveSink(const SharedRef<LogSink>& sink);

		/// @brief Copies this log's sinks to another log
		/// @param log The destination log
		void CopySinksTo(Log& log) const;

		/// @brief Writes a message to this log
		/// @param severity The severity of the message
		/// @param message The message
		void Write(LogMessageSeverity severity, const char* message);

		/// @brief Sets the minimum severity of messages that this log will write
		/// @param severity The minimum message severity
		void SetMinimumSeverity(LogMessageSeverity severity);

		/// @brief Gets the minimum message severity of this log
		/// @return The minimum message severity
		LogMessageSeverity GetMinimumSeverity() const { return _minSeverity; }

		/// @brief Writes a formatted message to this log
		/// @tparam ...Args The types of arguments
		/// @param severity The severity of the message
		/// @param format The message format
		/// @param ...args The arguments to write into the formatted message
		template<typename ... Args>
		void FormatWrite(LogMessageSeverity severity, const char* format, Args&&... args)
		{
			if (severity < _minSeverity)
				return;

			string message = FormatString(format, std::forward<Args>(args)...);
			Write(severity, message.c_str());
		}
	};
}

#ifdef COCO_LOG_TRACE
#define LogTrace(TargetLog, Message, ...) TargetLog->FormatWrite(Coco::LogMessageSeverity::Trace, Message, __VA_ARGS__);
#else
#define LogTrace(TargetLog, Message, ...)
#endif

#ifdef COCO_LOG_INFO
#define LogInfo(TargetLog, Message, ...) TargetLog->FormatWrite(Coco::LogMessageSeverity::Info, Message, __VA_ARGS__);
#else
#define LogInfo(TargetLog, Message, ...)
#endif

#ifdef COCO_LOG_WARNING
#define LogWarn(TargetLog, Message, ...) TargetLog->FormatWrite(Coco::LogMessageSeverity::Warning, Message, __VA_ARGS__);
#else
#define LogWarn(TargetLog, Message, ...)
#endif

#define LogError(TargetLog, Message, ...) TargetLog->FormatWrite(Coco::LogMessageSeverity::Error, Message, __VA_ARGS__);
#define LogCritical(TargetLog, Message, ...) TargetLog->FormatWrite(Coco::LogMessageSeverity::Critical, Message, __VA_ARGS__);