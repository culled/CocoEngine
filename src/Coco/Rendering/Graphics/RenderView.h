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
		/// <summary>
		/// The offset to begin rendering
		/// </summary>
		const Vector2Int RenderOffset;

		/// <summary>
		/// The size of the render
		/// </summary>
		const SizeInt RenderSize;

		/// <summary>
		/// The clear color for the render
		/// </summary>
		const Color ClearColor;

		RenderView(const Vector2Int& offset, const SizeInt& size, const Color& clearColor);
	};
}