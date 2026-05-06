//
// Created by cullen on 2/24/26.
//

#include "Logger.h"

#include <cstdarg>
#include <cstdio>

#include "Coco/Core/Types/DateTime.h"

namespace Coco
{
    Logger::Logger(LogMessageSeverity minSeverity) :
        _minSeverity(minSeverity),
        _sinks()
    {}

    void Logger::AddSink(SharedPtr<LogSink> sink)
    {
        if (_sinks.GetCount() == _maxSinkCount)
            throw Exception("Too many sinks"); // TODO: proper error

        _sinks.Append(sink);
    }

    void Logger::RemoveSink(const SharedPtr<LogSink>& sink)
    {
        _sinks.Remove(sink);
    }

    void Logger::Log(LogMessageSeverity severity, const char* messageFormat, ...)
    {
        if (severity < _minSeverity)
            return;

        char formattedMessage[MaxMessageLength]{ '\0' };

        // Format the variable arguments into the message
        va_list args;
        va_start(args, messageFormat);
        vsnprintf(formattedMessage, MaxMessageLength - 1, messageFormat, args);
        va_end(args);

        DateTime time = DateTime::Now();

        // Now format the rest of the message
        char finalMessage[MaxMessageLength + 100]{ '\0' };
        sprintf(finalMessage, "[%02d:%02d:%02d.%03d] (%s): %s",
            time.GetHour(),
            time.GetMinute(),
            time.GetSecond(),
            time.GetMillisecond(),
            ToString(severity),
            formattedMessage);

        for (auto& sink : _sinks)
        {
            sink->Write(severity, finalMessage);
        }
    }

    void Logger::SetMinimumSeverity(LogMessageSeverity minSeverity)
    {
        _minSeverity = minSeverity;
    }
}
