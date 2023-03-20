#pragma once

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

		IGraphicsSemaphore(const IGraphicsSemaphore&) = delete;
		IGraphicsSemaphore(IGraphicsSemaphore&&) = delete;

		IGraphicsSemaphore& operator=(const IGraphicsSemaphore&) = delete;
		IGraphicsSemaphore& operator=(IGraphicsSemaphore&&) = delete;
	};
}