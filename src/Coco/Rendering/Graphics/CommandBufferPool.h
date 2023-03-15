#pragma once

#include <Coco/Core/Core.h>
#include "CommandBuffer.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A pool of CommandBuffers
	/// </summary>
	class COCOAPI CommandBufferPool
	{
	protected:
		CommandBufferPool() = default;

	public:
		virtual ~CommandBufferPool() = default;

		/// <summary>
		/// Allocates a command buffer
		/// </summary>
		/// <param name="isPrimary">If true, the buffer will be a top-level buffer</param>
		/// <returns>A command buffer</returns>
		virtual CommandBuffer* Allocate(bool isPrimary) = 0;

		/// <summary>
		/// Frees a command buffer.
		/// NOTE: this will invalidate the buffer's handle
		/// </summary>
		/// <param name="buffer">The buffer to free</param>
		virtual void Free(CommandBuffer* buffer) noexcept = 0;

		/// <summary>
		/// Waits for this pool's queue to finish any asyncronous work
		/// </summary>
		virtual void WaitForQueue() noexcept = 0;
	};
}