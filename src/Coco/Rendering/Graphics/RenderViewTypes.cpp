#include "Renderpch.h"
#include "RenderViewTypes.h"
#include "../Mesh.h"
#include "Image.h"

namespace Coco::Rendering
{
	RenderTarget::RenderTarget() :
		RenderTarget(Ref<Image>(), Vector4::Zero)
	{}

	RenderTarget::RenderTarget(Ref<Image> mainImage, const Vector4& clearValue) :
		MainImage(mainImage),
		ResolveImage(),
		ClearValue(clearValue)
	{}

	RenderTarget::~RenderTarget()
	{
		MainImage.Invalidate();
		ResolveImage.Invalidate();
	}

	void RenderTarget::SetClearValues(std::span<RenderTarget> renderTargets, const Color& clearColor, double clearDepth, uint8 clearStencil)
	{
		for (RenderTarget& rt : renderTargets)
		{
			ImagePixelFormat pixelFormat = rt.MainImage->GetDescription().PixelFormat;

			if (IsDepthFormat(pixelFormat) || IsStencilFormat(pixelFormat))
			{
				rt.SetDepthStencilClearValue(clearDepth, clearStencil);
			}
			else
			{
				rt.SetColorClearValue(clearColor);
			}
		}
	}

	void RenderTarget::SetColorClearValue(const Color& clearColor)
	{
		Color linearColor = clearColor.AsLinear();
		ClearValue = Vector4(linearColor.R, linearColor.G, linearColor.B, linearColor.A);
	}

	void RenderTarget::SetDepthClearValue(double depthClearValue)
	{
		ClearValue.X = depthClearValue;
	}

	void RenderTarget::SetDepthStencilClearValue(double depthClearValue, uint8 stencilClearValue)
	{
		ClearValue.X = depthClearValue;
		ClearValue.Y = stencilClearValue;
	}

	RenderObjectData::RenderObjectData(
		uint64 id, 
		uint64 visibilityGroups, 
		SharedRef<Rendering::Mesh> mesh, 
		const Rendering::Submesh& submesh,
		SharedRef<Rendering::Material> material,
		const Matrix4x4& modelMatrix,
		const BoundingBox& bounds,
		std::any extraData) :
		ID(id),
		VisibilityGroups(visibilityGroups),
		Mesh(mesh),
		Submesh(submesh),
		Material(material),
		ModelMatrix(modelMatrix),
		Bounds(bounds),
		ExtraData(extraData)
	{}
}