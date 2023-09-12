#include "Corepch.h"
#include "ConsoleLogSink.h"

#include "../../Engine.h"

namespace Coco
{
	void ConsoleLogSink::Write(LogMessageSeverity severity, const char* message)
	{
		EnginePlatform* platform = Engine::Get()->GetPlatform();
		EnginePlatform::ConsoleColor color;

		switch (severity)
		{
		case Coco::LogMessageSeverity::Trace:
			color = EnginePlatform::ConsoleColor::White;
			break;
		case Coco::LogMessageSeverity::Info:
			color = EnginePlatform::ConsoleColor::Green;
			break;
		case Coco::LogMessageSeverity::Warning:
			color = EnginePlatform::ConsoleColor::Yellow;
			break;
		case Coco::LogMessageSeverity::Error:
			color = EnginePlatform::ConsoleColor::Red;
			break;
		case Coco::LogMessageSeverity::Critical:
			color = EnginePlatform::ConsoleColor::Magenta;
			break;
		default:
			color = EnginePlatform::ConsoleColor::White;
			break;
		}

		string line(message);
		line += "\n";
		
		platform->WriteToConsole(line.c_str(), color);
	}
}