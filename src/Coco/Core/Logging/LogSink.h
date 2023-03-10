#pragma once

#include <Coco/Core/Core.h>

#include "LogLevel.h"

namespace Coco::Logging
{
	/// <summary>
	/// A generic sink for log messages from a Logger
	/// </summary>
	class COCOAPI LogSink
	{
	public:
		/// <summary>
		/// The minimum level of messages that will be written to this sink
		/// </summary>
		const LogLevel MinimumLevel;

	public:
		LogSink() = delete;
		LogSink(const LogSink&) = delete;
		LogSink(LogSink&&) = delete;

		virtual ~LogSink() = default;

	protected:
		LogSink(LogLevel minimumLevel = LogLevel::Trace) noexcept;

	public:
		/// <summary>
		/// Writes to this log sink
		/// </summary>
		/// <param name="level">The level of the message</param>
		/// <param name="message">The message</param>
		virtual void Write(LogLevel level, const string& message) noexcept = 0;
	};
}

