#pragma once

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	/// @brief A semaphore that can be used to order operations on the GPU
	class COCOAPI IGraphicsSemaphore : public IGraphicsResource
	{
	protected:
		IGraphicsSemaphore() = default;

	public:
		virtual ~IGraphicsSemaphore() = default;

		IGraphicsSemaphore(const IGraphicsSemaphore&) = delete;
		IGraphicsSemaphore(IGraphicsSemaphore&&) = delete;

		IGraphicsSemaphore& operator=(const IGraphicsSemaphore&) = delete;
		IGraphicsSemaphore& operator=(IGraphicsSemaphore&&) = delete;
	};
}