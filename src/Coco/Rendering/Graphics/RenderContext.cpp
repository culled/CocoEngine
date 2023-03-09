#include "RenderContext.h"

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
	RenderContext::RenderContext(Ref<Rendering::RenderView> renderView) :
		RenderView(renderView)
	{
	}

	RenderContext::~RenderContext()
	{
		RenderView.reset();
	}

	void RenderContext::RestoreViewport()
	{
		SetViewport(RenderView->RenderOffset, RenderView->RenderSize);
	}

	void RenderContext::SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex)
	{
		CurrentRenderPass = renderPass;
		CurrentRenderPassIndex = passIndex;
	}
}