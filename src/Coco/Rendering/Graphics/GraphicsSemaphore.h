#pragma once

namespace Coco::Rendering
{
	/// @brief A semaphore that can be used to order operations on the GPU
	class GraphicsSemaphore
	{
	public:
		virtual ~GraphicsSemaphore() = default;
	};
}