#pragma once

#include <Coco/Core/Core.h>
#include "GraphicsResource.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A semaphore that can be used to order operations on the GPU
	/// </summary>
	class COCOAPI IGraphicsSemaphore : public IGraphicsResource
	{
	protected:
		IGraphicsSemaphore() = default;

	public:
		virtual ~IGraphicsSemaphore() = default;
	};
}