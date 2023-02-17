#pragma once

#include <Coco/Core/Logging/LogSink.h>

namespace Coco::Logging
{
    /// <summary>
    /// A LogSink that writes to a console window
    /// </summary>
    class COCOAPI ConsoleLogSink : public LogSink
    {
    public:
        ConsoleLogSink(LogLevel minimumLevel);

        virtual void Write(LogLevel level, const string& message) override;
    };
}