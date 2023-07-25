#pragma once

#include <Coco/Rendering/RenderingResource.h>

#include "Resources/CommandBuffer.h"

namespace Coco::Rendering
{
	/// @brief A pool that can allocate CommandBuffers
	class COCOAPI CommandBufferPool
	{
	protected:
		CommandBufferPool() = default;

	public:
		virtual ~CommandBufferPool() = default;

		/// @brief Allocates a command buffer
		/// @param isPrimary If true, the buffer will be a top-level buffer
		/// @return A command buffer
		virtual Ref<CommandBuffer> Allocate(bool isPrimary, const string& name = "CommandBuffer") = 0;

		/// @brief Frees a command buffer
		/// @param buffer The buffer to free
		virtual void Free(const Ref<CommandBuffer>& buffer) noexcept = 0;

		/// @brief Waits for this pool's queue to finish any asynchronous work
		virtual void WaitForQueue() noexcept = 0;
	};
}