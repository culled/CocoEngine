#include "FileExceptions.h"

namespace Coco
{
	FileException::FileException(const string& message) : Exception(message)
	{}

	FileOpenException::FileOpenException(const string& message) : FileException(message)
	{}
}