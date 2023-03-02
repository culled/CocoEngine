#include "Logger.h"

#include "LogSink.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include <Coco/Core/Types/TimeSpan.h>

namespace Coco::Logging
{
	/// <summary>
	/// Text representations of log levels
	/// </summary>
	const char* Logger::LogLevels[] = {
		"Trace",
		"Info",
		"Warning",
		"Error",
		"Fatal"
	};

	/// <summary>
	/// The number of log levels
	/// </summary>
	const int NumLogLevels = 5;

	Logger::Logger(const string& name) :
		Name(name)
	{}

	Logger::~Logger()
	{
		_logSinks.Clear();
	}

	void Logger::AddSink(Ref<LogSink> sink)
	{
		_logSinks.Add(std::forward<Ref<LogSink>>(sink));
	}

	void Logger::RemoveSink(const Ref<LogSink>& sink)
	{
		_logSinks.Remove(sink);
	}

	void Logger::Write(LogLevel level, const string& message)
	{
		// Safety if an invalid level was passed to us
		int levelIndex = std::clamp(static_cast<int>(level), 0, NumLogLevels - 1);
		level = static_cast<LogLevel>(levelIndex);

		const TimeSpan time = Engine::Get()->GetRunningTime();
		const string formattedMessage = FormattedString("[{:0>2}:{:0>2}:{:0>2}:{:0>3}] {} ({}): {}", 
			time.GetHours(), 
			time.GetMinutes(), 
			time.GetSeconds(), 
			time.GetMilliseconds(), 
			Name, 
			LogLevels[levelIndex], 
			message);

		// Write the full log message to all sinks with a lower minimum level than the message
		for (const Ref<LogSink>& sink : _logSinks)
		{
			if (sink->MinimumLevel <= level)
				sink->Write(level, formattedMessage);
		}
	}
}