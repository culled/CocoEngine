#include "RenderContext.h"

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
}