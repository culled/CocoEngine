//
// Created by cullen on 2/24/26.
//

#include "Exception.h"

#include <utility>
namespace Coco
{
    Exception::Exception(const char* message) :
        _message(message)
    {}

    Exception::Exception(String message) :
        _message(std::move(message))
    {}

    OutOfRangeException::OutOfRangeException(const char* message) :
        Exception(message)
    {}

    OutOfRangeException::OutOfRangeException(String message) :
        Exception(std::move(message))
    {}
}
