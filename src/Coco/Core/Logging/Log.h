#pragma once

#include "../Corepch.h"
#include "../Types/String.h"
#include "../Types/Refs.h"
#include "LogTypes.h"
#include "LogSink.h"

namespace Coco
{
	class Log
	{
	private:
		string _name;
		std::vector<SharedRef<LogSink>> _sinks;
		LogMessageSeverity _minSeverity;

	public:
		Log(const string& name, LogMessageSeverity minimumSeverity);
		~Log();

		void AddSink(const SharedRef<LogSink>& sink);
		void RemoveSink(const SharedRef<LogSink>& sink);
		void CopySinksTo(Log& log) const;
		void Write(LogMessageSeverity severity, const char* message);
		void SetMinimumSeverity(LogMessageSeverity severity);
		LogMessageSeverity GetMinimumSeverity() const { return _minSeverity; }

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