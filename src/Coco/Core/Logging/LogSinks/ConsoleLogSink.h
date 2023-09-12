#pragma once
#include "../LogSink.h"

namespace Coco
{
    class ConsoleLogSink : public LogSink
    {
    public:
        void Write(LogMessageSeverity severity, const char* message) final;
    };
}