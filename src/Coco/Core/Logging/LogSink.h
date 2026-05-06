//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_LOGSINK_H
#define COCOENGINE_LOGSINK_H

#include "LoggerTypes.h"

namespace Coco
{
    /// @brief Base class for sinks that write messages from a Logger
    class LogSink
    {
    public:
        virtual ~LogSink() = default;

        /// @brief Writes a message to this sink
        /// @param severity The message severity
        /// @param message The message
        virtual void Write(LogMessageSeverity severity, const char* message) = 0;
    };
} // Coco

#endif //COCOENGINE_LOGSINK_H