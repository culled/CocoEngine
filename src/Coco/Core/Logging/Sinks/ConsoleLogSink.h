#pragma once

#include <Coco/Core/Logging/LogSink.h>

namespace Coco::Logging
{
    /// @brief A LogSink that writes to a console window
    class COCOAPI ConsoleLogSink final : public LogSink
    {
    public:
        ConsoleLogSink(LogLevel minimumLevel) noexcept;
        ~ConsoleLogSink() final = default;

        ConsoleLogSink() = delete;
        ConsoleLogSink(const ConsoleLogSink&) = delete;
        ConsoleLogSink(ConsoleLogSink&&) = delete;
        ConsoleLogSink operator=(const ConsoleLogSink&) = delete;
        ConsoleLogSink operator=(ConsoleLogSink&&) = delete;

        void Write(LogLevel level, const string& message) noexcept final;
    };
}