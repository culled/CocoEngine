#include "Exceptions.h"

namespace Coco
{
	Exception::Exception(const string& message) : std::exception(message.c_str())
	{}

	IndexOutOfRangeException::IndexOutOfRangeException(const string& message) : Exception(message)
	{}

	InvalidOperationException::InvalidOperationException(const string& message) : Exception(message)
	{}
}