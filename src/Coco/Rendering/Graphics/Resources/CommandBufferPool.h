#pragma once

#include "GraphicsResource.h"

#include "CommandBuffer.h"

namespace Coco::Rendering
{
	/// @brief A pool that can allocate CommandBuffers
	class COCOAPI CommandBufferPool : public IGraphicsResource
	{
	protected:
		CommandBufferPool() = default;

	public:
		virtual ~CommandBufferPool() = default;

		/// @brief Allocates a command buffer
		/// @param isPrimary If true, the buffer will be a top-level buffer
		/// @return A command buffer
		virtual WeakManagedRef<CommandBuffer> Allocate(bool isPrimary) = 0;

		/// @brief Frees a command buffer
		/// @param buffer The buffer to free
		virtual void Free(const WeakManagedRef<CommandBuffer>& buffer) noexcept = 0;

		/// @brief Waits for this pool's queue to finish any asynchronous work
		virtual void WaitForQueue() noexcept = 0;
	};
}