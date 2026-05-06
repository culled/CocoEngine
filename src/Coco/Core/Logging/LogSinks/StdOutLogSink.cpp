//
// Created by cullen on 2/24/26.
//

#include "StdOutLogSink.h"

#include <iostream>
#include <ostream>

namespace Coco
{
    void StdOutLogSink::Write(LogMessageSeverity severity, const char* message)
    {
        /*ConsoleColor color = ConsoleColor::White;

        switch (severity)
        {
            case LogMessageSeverity::Info:
                color = ConsoleColor::Cyan;
                break;
            case LogMessageSeverity::Warning:
                color = ConsoleColor::Yellow;
                break;
            case LogMessageSeverity::Error:
                color = ConsoleColor::Red;
                break;
            case LogMessageSeverity::Critical:
                color = ConsoleColor::Magenta;
                break;
            default:
                break;
        }*/

        if (severity < LogMessageSeverity::Error)
        {
            std::cout << message << std::endl;
        }
        else
        {
            std::cerr << message << std::endl;
        }
    }
} // Coco