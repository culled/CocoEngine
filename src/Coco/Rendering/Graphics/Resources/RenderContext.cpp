#include "RenderContext.h"

#include "../../Pipeline/IRenderPass.h"
#include "../../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	const ActiveRenderPass ActiveRenderPass::None = ActiveRenderPass();

	ActiveRenderPass::ActiveRenderPass() : RenderPass(Ref<IRenderPass>()), RenderPassIndex(Math::MaxValue<uint>())
	{}

	ActiveRenderPass::ActiveRenderPass(const Ref<IRenderPass>& renderPass, uint renderPassIndex) :
		RenderPass(renderPass), RenderPassIndex(renderPassIndex)
	{}

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
		_currentRenderPass = ActiveRenderPass::None;
		_globalUO = GlobalUniformObject(renderView.Get());

		return BeginImpl();
	}

	void RenderContext::End()
	{
		EndImpl();

		_currentRenderView = Ref<Rendering::RenderView>();
		_currentRenderPipeline = Ref<RenderPipeline>();
		_currentRenderPass = ActiveRenderPass::None;
	}

	void RenderContext::Reset()
	{
		_currentRenderPass = ActiveRenderPass::None;
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

	void RenderContext::SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex) noexcept
	{
		_currentRenderPass = ActiveRenderPass(renderPass, passIndex);
	}
}