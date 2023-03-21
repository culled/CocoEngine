#include "RenderView.h"

namespace Coco::Rendering
{
	RenderView::RenderView(const RectInt& viewportRect, const Color& clearColor, const Matrix4x4& projection, const Matrix4x4& view) noexcept :
		ViewportRect(viewportRect), ClearColor(clearColor), Projection(projection), View(view)
	{
	}
}