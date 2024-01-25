#pragma once

#include <Coco/Core/Types/CoreExceptions.h>

namespace Coco::Rendering
{
	/// @brief An exception thrown if the RenderService failed to initialize
	class RenderingInitException :
		public Exception
	{
	public:
		RenderingInitException(const string& message);
	};
}

