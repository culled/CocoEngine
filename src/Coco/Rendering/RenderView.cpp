#include "RenderView.h"

namespace Coco::Rendering
{
	RenderView::RenderView(const Vector2Int& offset, const SizeInt& size, const Color& clearColor, const Matrix4x4& projection, const Matrix4x4& view) :
		RenderOffset(offset), RenderSize(size), ClearColor(clearColor), Projection(projection), View(view)
	{
	}
}