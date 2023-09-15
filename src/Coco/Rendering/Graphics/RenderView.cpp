#include "Renderpch.h"
#include "RenderView.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image) : 
		RenderTarget(image, Color::ClearBlack)
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Vector4 clearValue) :
		Image(image),
		ClearValue(clearValue)
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Color clearColor) :
		Image(image)
	{
		Color gammaColor = clearColor.AsGamma();
		ClearValue = Vector4(gammaColor.R, gammaColor.G, gammaColor.B, gammaColor.A);
	}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, double depthClearValue) :
		RenderTarget(image, Vector4(depthClearValue, 0, 0, 0))
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Vector2 depthStencilClearValue) :
		RenderTarget(image, Vector4(depthStencilClearValue.X, depthStencilClearValue.Y, 0, 0))
	{}

	MeshData::MeshData(const Ref<Buffer>& vertexBuffer, uint32 vertexCount, const Ref<Buffer>& indexBuffer, uint32 firstIndexOffset, uint32 indexCount) :
		VertexBuffer(vertexBuffer),
		VertexCount(vertexCount),
		IndexBuffer(indexBuffer),
		FirstIndexOffset(firstIndexOffset),
		IndexCount(indexCount)
	{}

	RenderView::RenderView(const RectInt& viewportRect, const RectInt& scissorRect, const std::vector<RenderTarget>& renderTargets) :
		_viewportRect(viewportRect),
		_scissorRect(scissorRect),
		_renderTargets(renderTargets)
	{}

	RenderTarget& RenderView::GetRenderTarget(size_t index)
	{
		Assert(index < _renderTargets.size())

		return _renderTargets.at(index);
	}

	const MeshData& RenderView::GetMeshData(uint64 key) const
	{
		Assert(_meshDatas.contains(key))

		return _meshDatas.at(key);
	}
}