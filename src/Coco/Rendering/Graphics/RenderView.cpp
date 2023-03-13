#include "RenderView.h"
#include <Coco/Rendering/Texture.h>

namespace Coco::Rendering
{
	RenderView::RenderView(const Vector2Int& offset, const SizeInt& size, const Color& clearColor, const Matrix4x4& projection, const Matrix4x4& view) :
		RenderOffset(offset), RenderSize(size), ClearColor(clearColor), Projection(projection), View(view)
	{
	}

	void RenderView::AddRenderObject(const GeometryRenderData& renderData)
	{
		RenderObjects.Add(renderData);
	}
}