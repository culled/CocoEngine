//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_LOGGERTYPES_H
#define COCOENGINE_LOGGERTYPES_H
#include "../Types/CoreTypes.h"

namespace Coco
{
    /// @brief Severity level of a log message
    enum class LogMessageSeverity : uint8
    {
        Verbose = 0,
        Info,
        Warning,
        Error,
        Critical
    };

    /// @brief Gets the string representation of a LogMessageSeverity level
    /// @param severity The severity
    /// @return The string representation
    const char* ToString(LogMessageSeverity severity) noexcept;
}
#endif //COCOENGINE_LOGGERTYPES_H