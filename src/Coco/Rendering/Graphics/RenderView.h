#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Contains all neccessary information to render a scene from a specific view
	/// </summary>
	struct COCOAPI RenderView
	{
		/// <summary>
		/// The viewport rectangle
		/// </summary>
		const RectInt ViewportRect;

		/// <summary>
		/// The clear color for the render
		/// </summary>
		const Color ClearColor;

		/// <summary>
		/// The projection matrix used for rendering
		/// </summary>
		const Matrix4x4 Projection;

		/// <summary>
		/// The view matrix used for rendering
		/// </summary>
		const Matrix4x4 View;

		RenderView(const RectInt& viewportRect, const Color& clearColor, const Matrix4x4& projection, const Matrix4x4& view) noexcept;
	};
}