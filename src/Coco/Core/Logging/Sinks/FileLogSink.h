#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/LogSink.h>
#include <Coco/Core/IO/File.h>

namespace Coco::Logging
{
	/// <summary>
	/// A LogSink that writes to a file
	/// </summary>
	class COCOAPI FileLogSink final : public LogSink
	{
	private:
		Managed<File> _file;

	public:
		FileLogSink(LogLevel minimumLevel, const string& file);
		~FileLogSink() final;

		FileLogSink() = delete;
		FileLogSink(const FileLogSink&) = delete;
		FileLogSink(FileLogSink&&) = delete;

		FileLogSink operator=(const FileLogSink&) = delete;
		FileLogSink operator=(FileLogSink&&) = delete;

		void Write(LogLevel level, const string& message) noexcept final;
	};
}