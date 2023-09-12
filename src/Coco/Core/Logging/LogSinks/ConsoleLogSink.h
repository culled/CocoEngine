#pragma once
#include "../LogSink.h"

namespace Coco
{
    /// @brief A LogSink that writes to the console window
    class ConsoleLogSink : public LogSink
    {
    public:
        void Write(LogMessageSeverity severity, const char* message) final;
    };
}