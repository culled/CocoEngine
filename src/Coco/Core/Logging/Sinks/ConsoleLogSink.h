#pragma once

#include <Coco/Core/Logging/LogSink.h>

namespace Coco::Logging
{
    /// <summary>
    /// A LogSink that writes to a console window
    /// </summary>
    class COCOAPI ConsoleLogSink final : public LogSink
    {
    public:
        ConsoleLogSink(LogLevel minimumLevel) noexcept;

        ConsoleLogSink() = delete;
        ConsoleLogSink(const ConsoleLogSink&) = delete;
        ConsoleLogSink(ConsoleLogSink&&) = delete;
        ~ConsoleLogSink() final = default;

        ConsoleLogSink operator=(const ConsoleLogSink&) = delete;
        ConsoleLogSink operator=(ConsoleLogSink&&) = delete;

        void Write(LogLevel level, const string& message) noexcept final;
    };
}