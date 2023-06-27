#include "RenderContext.h"

#include "../../Pipeline/IRenderPass.h"
#include "../../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	RenderContext::RenderContext(ResourceID id, const string& name, uint64_t lifetime) : RenderingResource(id, name, lifetime)
	{}

	bool RenderContext::Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline)
	{
		if (!IsAvaliableForRendering())
		{
			WaitForRenderingCompleted();
		}

		Reset();

		CurrentPipeline = pipeline;
		RenderView = renderView;
		GlobalUO = GlobalUniformObject(renderView.Get());

		return BeginImpl();
	}

	void RenderContext::End()
	{
		EndImpl();

		RenderView = Ref<Rendering::RenderView>();
		CurrentPipeline = Ref<RenderPipeline>();
		CurrentRenderPass = Ref<IRenderPass>();
	}

	void RenderContext::Reset()
	{
		CurrentRenderPassIndex = 0;

		DrawCallCount = 0;
		TrianglesDrawn = 0;

		ResetImpl();
	}

	void RenderContext::RestoreViewport()
	{
		SetViewport(RenderView->ViewportRect);
	}

	void RenderContext::GetLastFrameStats(uint64_t& drawCallCount, uint64_t trianglesDrawn) const noexcept
	{
		drawCallCount = DrawCallCount;
		trianglesDrawn = TrianglesDrawn;
	}

	void RenderContext::SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex) noexcept
	{
		CurrentRenderPass = renderPass;
		CurrentRenderPassIndex = passIndex;
	}
}