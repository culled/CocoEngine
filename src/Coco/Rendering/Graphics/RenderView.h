#pragma once
#include "Image.h"
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Rendering
{
	/// @brief An image that can be rendered to
	struct RenderTarget
	{
		/// @brief The actual image that will be rendered to
		Image* Image;

		/// @brief Clear values for clearing the image the first time it is used (if it needs to be cleared)
		Vector4 ClearValue;

		RenderTarget(Rendering::Image* image);
		RenderTarget(Rendering::Image* image, Vector4 clearValue);
		RenderTarget(Rendering::Image* image, Color clearColor);
		RenderTarget(Rendering::Image* image, double depthClearValue);
		RenderTarget(Rendering::Image* image, Vector2 depthStencilClearValue);
	};

	/// @brief Holds information needed to render a scene
	class RenderView
	{
	private:
		RectInt _viewportRect;
		RectInt _scissorRect;
		std::vector<RenderTarget> _renderTargets;

	public:
		RenderView(
			const RectInt& viewportRect, 
			const RectInt& scissorRect, 
			const std::vector<RenderTarget>& renderTargets);

		/// @brief Gets the viewport rectangle
		/// @return The viewport rectangle
		const RectInt& GetViewportRect() const { return _viewportRect; }

		/// @brief Gets the scissor rectangle (actual part of viewport that is rendered)
		/// @return The scissor rectangle
		const RectInt& GetScissorRect() const { return _scissorRect; }

		/// @brief Gets the render targets
		/// @return The render targets
		const std::vector<RenderTarget>& GetRenderTargets() const { return _renderTargets; }

		/// @brief Gets a single render target
		/// @param index The index of the render target
		/// @return The render target at the given index
		RenderTarget& GetRenderTarget(size_t index);
	};
}