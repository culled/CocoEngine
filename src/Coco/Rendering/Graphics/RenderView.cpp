#include "Renderpch.h"
#include "RenderView.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderTarget::RenderTarget(Rendering::Image* image) : 
		RenderTarget(image, Color::ClearBlack)
	{}

	RenderTarget::RenderTarget(Rendering::Image* image, Vector4 clearValue) :
		Image(image),
		ClearValue(clearValue)
	{}

	RenderTarget::RenderTarget(Rendering::Image * image, Color clearColor) :
		Image(image)
	{
		Color gammaColor = clearColor.AsGamma();
		ClearValue = Vector4(gammaColor.R, gammaColor.G, gammaColor.B, gammaColor.A);
	}

	RenderTarget::RenderTarget(Rendering::Image* image, double depthClearValue) :
		RenderTarget(image, Vector4(depthClearValue, 0, 0, 0))
	{}

	RenderTarget::RenderTarget(Rendering::Image* image, Vector2 depthStencilClearValue) :
		RenderTarget(image, Vector4(depthStencilClearValue.X, depthStencilClearValue.Y, 0, 0))
	{}

	RenderView::RenderView(const RectInt& viewportRect, const RectInt& scissorRect, const std::vector<RenderTarget>& renderTargets) :
		_viewportRect(viewportRect),
		_scissorRect(scissorRect),
		_renderTargets(renderTargets)
	{}

	RenderTarget& RenderView::GetRenderTarget(size_t index)
	{
		if (index >= _renderTargets.size())
		{
			string message = FormatString("Invalid RenderTarget index. 0<{}<{} == false", index, _renderTargets.size());
			throw std::exception(message.c_str());
		}

		return _renderTargets.at(index);
	}
}