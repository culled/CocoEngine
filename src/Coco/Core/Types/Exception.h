//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_EXCEPTION_H
#define COCOENGINE_EXCEPTION_H

#include "String.h"
#include <exception>

namespace Coco
{
    /// @brief Base class for all exceptions
    class Exception :
        public std::exception
    {
    public:
        Exception(const char* message);
        Exception(String message);

        const char* what() const noexcept override { return _message.CStr(); }

    private:
        String _message;
    };

    /// @brief An exception that is thrown when a value is outside an expected range
    class OutOfRangeException :
        public Exception
    {
    public:
        OutOfRangeException(const char* message);
        OutOfRangeException(String message);
    };
}

#endif //COCOENGINE_EXCEPTION_H