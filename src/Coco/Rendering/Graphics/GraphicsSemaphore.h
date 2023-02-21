#pragma once

#include <Coco/Core/Core.h>

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// <summary>
	/// A semaphore that can be used to order operations on the GPU
	/// </summary>
	class COCOAPI GraphicsSemaphore : public GraphicsResource
	{
	protected:
		GraphicsSemaphore(GraphicsDevice* owningDevice) : GraphicsResource(owningDevice) {}

	public:
		virtual ~GraphicsSemaphore() = default;
	};
}