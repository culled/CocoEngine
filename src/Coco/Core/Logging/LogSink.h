#pragma once

#include "LogTypes.h"

namespace Coco
{
	/// @brief A sink that writes log messages
	class LogSink
	{
	public:
		virtual ~LogSink() = default;

		/// @brief Writes a message to this sink
		/// @param severity The severity of the message
		/// @param message The message
		virtual void Write(LogMessageSeverity severity, const char* message) = 0;
	};
}