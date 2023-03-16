#include "Exceptions.h"

namespace Coco
{
	Exception::Exception(const string& message) : std::exception(message.c_str())
	{}
}