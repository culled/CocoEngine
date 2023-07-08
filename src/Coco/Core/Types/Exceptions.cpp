#include "Exceptions.h"

namespace Coco
{
	Exception::Exception(const string& message) noexcept : std::exception(message.c_str())
	{}

	IndexOutOfRangeException::IndexOutOfRangeException(const string& message) noexcept : Exception(message)
	{}

	InvalidOperationException::InvalidOperationException(const string& message) noexcept : Exception(message)
	{}
}