#include "EnginePlatformExceptions.h"

namespace Coco::Platform
{
	EnginePlatformException::EnginePlatformException(const string& message) : Exception(message)
	{}

	EnginePlatformInitializeException::EnginePlatformInitializeException(const string& message) : EnginePlatformException(message)
	{}

	EnginePlatformOperationException::EnginePlatformOperationException(const string& message) : EnginePlatformException(message)
	{}
}