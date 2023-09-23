#include "Corepch.h"
#include "Log.h"

#include "../Engine.h"

namespace Coco
{
	Log::Log(const string& name, LogMessageSeverity minimumSeverity) :
		_name(name),
		_minSeverity(minimumSeverity),
		_sinks{}
	{}

	Log::~Log()
	{
		_sinks.clear();
	}

	void Log::AddSink(const SharedRef<LogSink>& sink)
	{
		const auto it = FindSharedRef(_sinks.cbegin(), _sinks.cend(), sink);

		if (it != _sinks.cend())
			return;

		_sinks.push_back(sink);
	}

	void Log::RemoveSink(const SharedRef<LogSink>& sink)
	{
		const auto it = FindSharedRef(_sinks.cbegin(), _sinks.cend(), sink);

		if (it == _sinks.cend())
			return;

		_sinks.erase(it);
	}

	void Log::CopySinksTo(Log& log) const
	{
		for (const SharedRef<LogSink>& sink : _sinks)
			log.AddSink(sink);
	}

	void Log::Write(LogMessageSeverity severity, const char* message)
	{
		if (severity < _minSeverity)
			return;

		DateTime time = Engine::cGet()->GetPlatform().GetLocalTime();
		string formattedMessage = FormatString(
			"[{:0>2}:{:0>2}:{:0>2}:{:0>3}] {} ({}): {}",
			time.GetHour(),
			time.GetMinute(),
			time.GetSecond(),
			time.GetMillisecond(),
			_name,
			GetLogMessageSeverityString(severity),
			message);

		for (SharedRef<LogSink>& sink : _sinks)
			sink->Write(severity, formattedMessage.c_str());
	}

	void Log::SetMinimumSeverity(LogMessageSeverity severity)
	{
		_minSeverity = severity;
	}
}