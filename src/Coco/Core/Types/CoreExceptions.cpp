#include "Corepch.h"
#include "CoreExceptions.h"

namespace Coco
{
	Exception::Exception(const string& message) :
		std::exception(message.c_str())
	{}

	NotFoundException::NotFoundException(const string& message) :
		Exception(message)
	{}

	InvalidOperationException::InvalidOperationException(const string& message) :
		Exception(message)
	{}

	OutOfRangeException::OutOfRangeException(const string& message) :
		Exception(message)
	{}

	NotImplementedException::NotImplementedException() :
		NotImplementedException("This functionality has not been implemented")
	{}

	NotImplementedException::NotImplementedException(const string& message) :
		Exception(message)
	{}
}