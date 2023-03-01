#include "RenderView.h"

namespace Coco::Rendering
{
	RenderView::RenderView(const Vector2Int& offset, const SizeInt& size, const Color& clearColor) :
		RenderOffset(offset), RenderSize(size), ClearColor(clearColor)
	{
	}
}