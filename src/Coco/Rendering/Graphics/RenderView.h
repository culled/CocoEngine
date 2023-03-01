#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Contains all neccessary information to render a scene from a specific view
	/// </summary>
	struct COCOAPI RenderView
	{
		const Vector2Int RenderOffset;
		const SizeInt RenderSize;
		const Color ClearColor;

		RenderView(const Vector2Int& offset, const SizeInt& size, const Color& clearColor);
	};
}