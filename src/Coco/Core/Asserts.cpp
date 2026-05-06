//
// Created by cullen on 2/24/26.
//

#include "Asserts.h"
#include "Engine.h"

#include <cstdarg>
#include <cstdio>

#include "Logging/LoggerTypes.h"

namespace Coco
{
    void ReportAssertionFailure(const char* expression, const char* file, int32 line, const char* message, ...) noexcept
    {
        auto engine = Engine::Get();
        if (!engine)
            return;

        if (message)
        {
            constexpr int messageBufferSize = 1024;
            char formattedMessage[messageBufferSize]{ '\0' };

            // Format the variable arguments into the message
            va_list args;
            va_start(args, message);
            vsnprintf(formattedMessage, messageBufferSize - 1, message, args);
            va_end(args);

            LOG_CRITICAL(engine->GetLogger(), "Assertion failed - %s: %s, in file %s, line %d", formattedMessage, expression, file, line);
        }
        else
        {
            LOG_CRITICAL(engine->GetLogger(), "Assertion failed: %s, in file %s, line %d", expression, file, line);
        }
    }
}
