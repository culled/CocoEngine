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
	const Array<const char*, 5> Logger::LogLevels = {
		"Trace",
		"Info",
		"Warning",
		"Error",
		"Fatal"
	};

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

	void Logger::Write(LogLevel level, const string& message) noexcept
	{
		// Safety if an invalid level was passed to us
		const int levelIndex = Math::Clamp(static_cast<int>(level), 0, static_cast<int>(LogLevels.size()));

		const TimeSpan time = Engine::Get()->GetRunningTime();
		const string formattedMessage = FormattedString("[{:0>2}:{:0>2}:{:0>2}:{:0>3}] {} ({}): {}", 
			time.GetHours(), 
			time.GetMinutes(), 
			time.GetSeconds(), 
			time.GetMilliseconds(), 
			Name, 
			LogLevels.at(levelIndex), 
			message);

		// Write the full log message to all sinks with a lower minimum level than the message
		for (const Ref<LogSink>& sink : _logSinks)
		{
			if (sink->MinimumLevel <= level)
				sink->Write(level, formattedMessage);
		}
	}
}