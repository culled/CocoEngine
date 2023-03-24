#pragma once

#include <Coco/Core/Core.h>

#include "LogLevel.h"

namespace Coco::Logging
{
	/// @brief A generic sink for log messages from a Logger
	class COCOAPI LogSink
	{
	public:
		/// @brief The minimum level of messages that will be written to this sink
		const LogLevel MinimumLevel;

	public:
		LogSink() = delete;
		LogSink(const LogSink&) = delete;
		LogSink(LogSink&&) = delete;

		virtual ~LogSink() = default;

	protected:
		LogSink(LogLevel minimumLevel = LogLevel::Trace) noexcept;

	public:
		/// @brief Writes to this log sink
		/// @param level The level of the message
		/// @param message The message 
		virtual void Write(LogLevel level, const string& message) noexcept = 0;
	};
}

