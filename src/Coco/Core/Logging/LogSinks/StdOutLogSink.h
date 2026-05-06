//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_STDOUTLOGSINK_H
#define COCOENGINE_STDOUTLOGSINK_H
#include "Coco/Core/Logging/LogSink.h"

namespace Coco
{
    /// @brief A LogSink that outputs to stdout
    class StdOutLogSink :
        public LogSink
    {
    public:
        void Write(LogMessageSeverity severity, const char* message) override;
    };
} // Coco

#endif //COCOENGINE_STDOUTLOGSINK_H