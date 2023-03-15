#include "LogSink.h"

namespace Coco::Logging
{
	LogSink::LogSink(LogLevel minimumLevel) noexcept :
		MinimumLevel(minimumLevel)
	{}
}