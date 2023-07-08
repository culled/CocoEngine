#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Array.h>

#include "LogLevel.h"
#include "LogSink.h"

#ifdef COCO_LOG_TRACE
// Logs a trace message
#define LogTrace(Logger, Message) Logger->Write(Coco::Logging::LogLevel::Trace, Message)
#else
// Logs a trace message
#define LogTrace(Logger, Message)
#endif

#ifdef COCO_LOG_INFO
// Logs an info message
#define LogInfo(Logger, Message) Logger->Write(Coco::Logging::LogLevel::Info, Message)
#else
// Logs an info message
#define LogInfo(Logger, Message)
#endif

#ifdef COCO_LOG_WARNING
// Logs a warning message
#define LogWarning(Logger, Message) Logger->Write(Coco::Logging::LogLevel::Warning, Message)
#else
// Logs a warning message
#define LogWarning(Logger, Message)
#endif

// Logs an error message
#define LogError(Logger, Message) Logger->Write(Coco::Logging::LogLevel::Error, Message)

// Logs a fatal error message
#define LogFatal(Logger, Message) Logger->Write(Coco::Logging::LogLevel::Fatal, Message)

#if COCO_ASSERTIONS
// Asserts and writes a failure to a log with a message
#define LogAssertMessage(Logger, Expression, Message)																									\
{																																						\
	if(Expression)																																		\
	{ } else																																			\
	{																																					\
		LogFatal(Logger, Coco::FormattedString("Assertion failed for {0} in file {1} at line {2}\n\t- {3}", #Expression, __FILE__, __LINE__, Message));	\
		DebuggerBreak();																																\
	}																																					\
}

// Asserts and writes a failure to a log
#define LogAssert(Logger, Expression) LogAssertMessage(Logger, Expression, "")
#else
// Asserts and writes a failure to a log with a message
#define LogAssertMessage(Logger, Expression, Message)

// Asserts and writes a failure to a log
#define LogAssert(Logger, Expression)
#endif

namespace Coco::Logging
{
	/// @brief A log that can write messages to various LogSinks
	class COCOAPI Logger
	{
	private:
		List<SharedRef<LogSink>> _logSinks;

	public:
		/// @brief Text representations of log levels
		static const Array<const char*, 5> LogLevels;

		/// @brief The name of this logger
		const string Name;

	public:
		Logger(const string& name);
		virtual ~Logger();

		Logger() = delete;
		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;

		Logger& operator=(const Logger&) = delete;
		Logger& operator=(Logger&&) = delete;

		/// @brief Adds a sink to this logger
		/// @param sink The sink to add
		void AddSink(SharedRef<LogSink> sink);

		/// @brief Removes a sink from this logger
		/// @param sink The sink to remove
		void RemoveSink(const SharedRef<LogSink>& sink) noexcept;

		/// @brief Writes a message with a log level to this logger's sinks
		/// @param level The level of the message
		/// @param message The message
		void Write(LogLevel level, const string& message) noexcept;
	};
}

