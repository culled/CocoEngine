#include "FileLogSink.h"

namespace Coco::Logging
{
	FileLogSink::FileLogSink(LogLevel minimumLevel, const string& file) : LogSink(minimumLevel)
	{
		_file = CreateManaged<File>(file, FileModes::Write);
	}

	FileLogSink::~FileLogSink()
	{
		try
		{
			_file->Close();
		}
		catch (...)
		{}

		_file.reset();
	}

	void FileLogSink::Write(LogLevel level, const string& message) noexcept
	{
		try
		{
			// TODO: write on another thread to prevent performance drops
			_file->WriteLine(message);
		}
		catch(...)
		{}
	}
}