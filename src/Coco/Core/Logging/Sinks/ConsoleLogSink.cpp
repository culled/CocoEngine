#include "ConsoleLogSink.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/EnginePlatform.h>

namespace Coco::Logging
{
	ConsoleLogSink::ConsoleLogSink(LogLevel minimumLevel) : LogSink(minimumLevel)
	{}

	void ConsoleLogSink::Write(LogLevel level, const string & message)
	{
		Platform::ConsoleColor color;

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
			color = Platform::ConsoleColor::White;
			break;
		}

		Engine::Get()->GetPlatform()->WriteToPlatformConsole(message + "\n", color, level >= LogLevel::Error);
	}
}