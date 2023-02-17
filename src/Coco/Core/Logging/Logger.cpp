#include "Logger.h"

#include "LogSink.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/EnginePlatform.h>
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
		int levelIndex = static_cast<int>(level);

		if (levelIndex < 0 || levelIndex > NumLogLevels - 1)
		{
			throw IndexOutOfRangeException(FormattedString("Log level was invalid: 0 <= {0} <= 4", levelIndex));
		}

		const TimeSpan time = Engine::Get()->GetRunningTime();
		const string timeString = FormattedString("{:0>2}:{:0>2}:{:0>2}:{:0>3}", time.GetHours(), time.GetMinutes(), time.GetSeconds(), time.GetMilliseconds());

		// TODO: get time
		const string formattedMessage = FormattedString("[{}] {} ({}): {}", timeString, Name, LogLevels[levelIndex], message);

		for (const Ref<LogSink>& sink : _logSinks)
		{
			if (sink->MinimumLevel <= level)
				sink->Write(level, formattedMessage);
		}
	}
}