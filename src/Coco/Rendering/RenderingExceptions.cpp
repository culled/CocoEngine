#include "Renderpch.h"
#include "RenderingExceptions.h"

namespace Coco::Rendering
{
	RenderingInitException::RenderingInitException(const string& message) :
		Exception(message)
	{}
}