#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Color.h>

namespace Coco::Rendering
{
	/// @brief Contains all neccessary information to render a scene from a specific view
	struct COCOAPI RenderView
	{
		/// @brief The viewport rectangle
		const RectInt ViewportRect;

		/// @brief The clear color for the render
		const Color ClearColor;

		/// @brief The projection matrix used for rendering
		const Matrix4x4 Projection;

		/// @brief The view matrix used for rendering
		const Matrix4x4 View;

		RenderView(const RectInt& viewportRect, const Color& clearColor, const Matrix4x4& projection, const Matrix4x4& view) noexcept;
	};
}