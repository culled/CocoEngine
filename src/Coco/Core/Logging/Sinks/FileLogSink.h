#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/LogSink.h>
#include <Coco/Core/IO/File.h>

namespace Coco::Logging
{
	/// <summary>
	/// A LogSink that writes to a file
	/// </summary>
	class COCOAPI FileLogSink : public LogSink
	{
	private:
		Managed<File> _file;

	public:
		FileLogSink(LogLevel minimumLevel, const string& file);
		virtual ~FileLogSink() override;

		virtual void Write(LogLevel level, const string& message) override;
	};
}