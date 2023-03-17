#include "WindowExceptions.h"

namespace Coco::Windowing
{
	WindowException::WindowException(const string& message) : Exception(message)
	{
	}

	WindowCreateException::WindowCreateException(const string& message) : WindowException(message)
	{
	}

	WindowOperationException::WindowOperationException(const string& message) : WindowException(message)
	{
	}
}
