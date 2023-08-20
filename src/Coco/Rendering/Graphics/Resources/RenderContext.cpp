#include "RenderContext.h"

#include "../../Pipeline/IRenderPass.h"
#include "../../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	RenderContext::RenderContext(const ResourceID& id, const string& name) : RenderingResource(id, name)
	{}

	bool RenderContext::Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline)
	{
		if (!IsAvaliableForRendering())
		{
			WaitForRenderingCompleted();
		}

		Reset();

		_currentRenderPipeline = pipeline;
		_currentRenderView = renderView;
		_globalUO = GlobalUniformObject(renderView.Get());

		return BeginImpl();
	}

	bool RenderContext::BeginPass(Ref<IRenderPass> renderPass)
	{
		_currentRenderPassIndex++;
		_currentRenderPass = renderPass;

		return BeginPassImpl();
	}

	void RenderContext::End()
	{
		EndImpl();

		_currentRenderView = Ref<Rendering::RenderView>::Empty;
		_currentRenderPipeline = Ref<RenderPipeline>::Empty;
		_currentRenderPass = Ref<IRenderPass>::Empty;
		_currentRenderPassIndex = -1;
	}

	void RenderContext::Reset()
	{
		_currentRenderPass = Ref<IRenderPass>::Empty;
		_currentRenderPassIndex = -1;
		_currentDrawCallCount = 0;
		_currentTrianglesDrawn = 0;

		ResetImpl();
	}

	void RenderContext::RestoreViewport()
	{
		SetViewport(_currentRenderView->ViewportRect);
	}

	void RenderContext::GetLastFrameStats(uint64_t& drawCallCount, uint64_t trianglesDrawn) const noexcept
	{
		drawCallCount = _currentDrawCallCount;
		trianglesDrawn = _currentTrianglesDrawn;
	}
}