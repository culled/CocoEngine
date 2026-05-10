//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_LOGGER_H
#define COCOENGINE_LOGGER_H
#include "LoggerTypes.h"
#include "LogSink.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Types/StackArray.h"

namespace Coco
{
    /// @brief Logs messages to a number of LogSinks
    class Logger
    {
    public:
        /// @brief The maximum message length, in characters
        static constexpr int MaxMessageLength = 1023;

        Logger(LogMessageSeverity minSeverity);

        /// @brief Creates a sink and adds it to this log's list of sinks
        /// @tparam SinkType The type of sink to create
        /// @tparam Args The args to pass to the sink's constructor
        /// @param args The args to pass to the sink's constructor
        /// @return The created sink
        template<typename SinkType, typename... Args>
        SharedPtr<SinkType> CreateSink(Args&&... args)
        {
            auto sink = CreateDefaultShared<SinkType>(std::forward<Args>(args)...);
            AddSink(sink);
            return sink;
        }

        /// @brief Adds a sink to this Logger
        /// @param sink The sink
        void AddSink(SharedPtr<LogSink> sink);

        /// @brief Removes a sink from this Logger
        /// @param sink The sink
        void RemoveSink(const SharedPtr<LogSink>& sink);

        /// @brief Logs a formatted message
        /// @param severity The severity of the message
        /// @param messageFormat The message with sprintf formatting
        /// @param ... Parameters to format the message with
        void Log(LogMessageSeverity severity, const char* messageFormat, ...);

        /// @brief Sets the minimum severity of messages that will be logged.
        /// Messages with severities lower than this value will not be logged to this Logger's sinks
        /// @param minSeverity The minimum severity of message that will be logged by this Logger
        void SetMinimumSeverity(LogMessageSeverity minSeverity);

        /// @brief Gets the minimum severity of message that this Logger will log
        /// @return The minimum message severity
        LogMessageSeverity GetMinimumSeverity() const { return _minSeverity; }

    private:
        static constexpr int _maxSinkCount = 4;

        LogMessageSeverity _minSeverity;
        StackArray<SharedPtr<LogSink>, _maxSinkCount> _sinks;
    };
}

#define LOG_VERBOSE(Logger, Message, ...) if(Logger) Logger->Log(Coco::LogMessageSeverity::Verbose, Message, ##__VA_ARGS__)
#define LOG_INFO(Logger, Message, ...) if(Logger) Logger->Log(Coco::LogMessageSeverity::Info, Message, ##__VA_ARGS__)
#define LOG_WARN(Logger, Message, ...) if(Logger) Logger->Log(Coco::LogMessageSeverity::Warning, Message, ##__VA_ARGS__)
#define LOG_ERROR(Logger, Message, ...) if(Logger) Logger->Log(Coco::LogMessageSeverity::Error, Message, ##__VA_ARGS__)
#define LOG_CRITICAL(Logger, Message, ...) if(Logger) Logger->Log(Coco::LogMessageSeverity::Critical, Message, ##__VA_ARGS__)

#endif //COCOENGINE_LOGGER_H
