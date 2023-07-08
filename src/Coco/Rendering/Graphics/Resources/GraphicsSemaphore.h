#pragma once

#include <Coco/Rendering/RenderingResource.h>

namespace Coco::Rendering
{
	/// @brief A semaphore that can be used to order operations on the GPU
	class COCOAPI GraphicsSemaphore : public RenderingResource
	{
	public:
		GraphicsSemaphore(ResourceID id, const string& name) : RenderingResource(id, name) {}
		virtual ~GraphicsSemaphore() = default;

		GraphicsSemaphore(const GraphicsSemaphore&) = delete;
		GraphicsSemaphore(GraphicsSemaphore&&) = delete;

		GraphicsSemaphore& operator=(const GraphicsSemaphore&) = delete;
		GraphicsSemaphore& operator=(GraphicsSemaphore&&) = delete;
	};
}