#include "Renderpch.h"
#include "RenderContextPool.h"

namespace Coco::Rendering
{
	RenderContextPool::RenderContextPool(GraphicsDevice& device, uint64 maxContexts) :
		_device(device),
		_maxContexts(maxContexts),
		_renderContexts(),
		_orphanedContexts()
	{}

	RenderContextPool::~RenderContextPool()
	{
		while (!_orphanedContexts.empty())
			_orphanedContexts.pop();

		for (auto& context : _renderContexts)
		{
			_device.TryReleaseRenderContext(context);
		}

		_renderContexts.clear();
	}

	Ref<RenderContext> RenderContextPool::Get(bool addWaitOnSemaphore)
	{
		Ref<RenderContext> context;

		auto it = std::find_if(_renderContexts.begin(), _renderContexts.end(), [](auto& c) { return c->CheckForRenderingComplete(); });
		
		if (it != _renderContexts.end())
		{
			context = *it;

			context->Reset();
		}
		else
		{
			if (_renderContexts.size() < _maxContexts)
			{
				context = _device.CreateRenderContext();
				_renderContexts.push_back(context);
			}
			else
			{
				context = _renderContexts.front();
				context->WaitForRenderingToComplete();
				context->Reset();
			}
		}

		if (addWaitOnSemaphore && !_orphanedContexts.empty() && context->GetWaitOnSemaphoreCount() == 0)
		{
			context->AddWaitOnSemaphore(_orphanedContexts.front()->GetRenderCompletedSemaphore());
			_orphanedContexts.pop();
		}

		return context;
	}

	void RenderContextPool::MarkOrphan(Ref<RenderContext>& context)
	{
		_orphanedContexts.push(context);
	}
}