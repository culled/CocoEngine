#pragma once

#include <Coco/Core/Types/Refs.h>
#include "GraphicsDevice.h"
#include "RenderContext.h"

namespace Coco::Rendering
{
	/// @brief A pool for RenderContexts
	class RenderContextPool
	{
	private:
		GraphicsDevice& _device;
		uint64 _maxContexts;
		std::vector<Ref<RenderContext>> _renderContexts;
		std::queue<Ref<RenderContext>> _orphanedContexts;

	public:
		RenderContextPool(GraphicsDevice& device, uint64 maxContexts);
		~RenderContextPool();

		/// @brief Gets a RenderContext that can be used for rendering
		/// @param addWaitOnSemaphore If true, the given context will have its wait-on semaphore set to an orphaned context's render completed semaphore, if any exist
		/// @return A RenderContext
		Ref<RenderContext> Get(bool addWaitOnSemaphore);

		/// @brief Marks the given context as an orphan, meaning that nothing consumed its render completed semaphore
		/// @param context The orphaned context
		void MarkOrphan(Ref<RenderContext>& context);
	};
}