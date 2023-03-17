#include "RenderingExceptions.h"

namespace Coco::Rendering
{
	RenderingException::RenderingException(const string& message) : Exception(message)
	{
	}

	RenderingOperationException::RenderingOperationException(const string& message) : RenderingException(message)
	{
	}

	GraphicsPlatformInitializeException::GraphicsPlatformInitializeException(const string& message) : RenderingException(message)
	{
	}

	BufferTransferException::BufferTransferException(const string& message) : RenderingException(message)
	{
	}

	SurfaceInitializationException::SurfaceInitializationException(const string& message) : RenderingException(message)
	{
	}
}