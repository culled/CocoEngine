//
// Created by cullen on 2/24/26.
//
#include "LoggerTypes.h"

namespace Coco
{
    const char* ToString(LogMessageSeverity severity) noexcept
    {
        switch (severity)
        {
            case LogMessageSeverity::Info:
                return "Info";
            case LogMessageSeverity::Warning:
                return "Warning";
            case LogMessageSeverity::Error:
                return "Error";
            case LogMessageSeverity::Critical:
                return "Critical";
            case LogMessageSeverity::Verbose:
            default:
                return "Verbose";
        }
    }
}
