#include "ConsoleLogSink.h"

#include <Coco/Core/Engine.h>

namespace Coco::Logging
{
	ConsoleLogSink::ConsoleLogSink(LogLevel minimumLevel) noexcept : LogSink(minimumLevel)
	{}

	void ConsoleLogSink::Write(LogLevel level, const string & message) noexcept
	{
		Platform::ConsoleColor color = Platform::ConsoleColor::White;

		switch (level)
		{	
		case Coco::Logging::LogLevel::Trace:
			color = Platform::ConsoleColor::Cyan;
			break;
		case Coco::Logging::LogLevel::Info:
			color = Platform::ConsoleColor::White;
			break;
		case Coco::Logging::LogLevel::Warning:
			color = Platform::ConsoleColor::Yellow;
			break;
		case Coco::Logging::LogLevel::Error:
			color = Platform::ConsoleColor::Red;
			break;
		case Coco::Logging::LogLevel::Fatal:
			color = Platform::ConsoleColor::Magenta;
			break;
		default:
			break;
		}

		try
		{
			Engine::Get()->GetPlatform()->WriteToConsole(message + "\n", color, level >= LogLevel::Error);
		}
		catch(...)
		{ }
	}
}