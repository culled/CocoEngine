#include "RenderContext.h"

namespace Coco::Rendering
{
	RenderContext::RenderContext(Rendering::RenderView* renderView) :
		RenderView(renderView)
	{
	}

	RenderContext::~RenderContext()
	{
		RenderView.reset();
	}
}