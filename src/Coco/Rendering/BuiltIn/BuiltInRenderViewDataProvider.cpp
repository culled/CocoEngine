#include "Renderpch.h"
#include "BuiltInRenderViewDataProvider.h"

#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include "../RenderService.h"

namespace Coco::Rendering
{
	BuiltInRenderViewDataProvider::BuiltInRenderViewDataProvider(
		const Color& clearColor, 
		const Vector2& depthStencilClearValues, 
		MSAASamples sampleCount, 
		std::optional<ShaderUniformLayout> globalUniformLayoutOverride) :
		_clearColor(clearColor),
		_depthStencilClearValues(depthStencilClearValues),
		_sampleCount(sampleCount),
		_globalUniformLayoutOverride(globalUniformLayoutOverride),
		_isPerspective(true),
		_projectionSize(Math::DegToRad(45.0)),
		_nearClip(0.1),
		_farClip(100),
		_viewTransform()
	{}

	void BuiltInRenderViewDataProvider::SetupRenderView(
		RenderView& renderView, 
		const CompiledRenderPipeline& pipeline, 
		uint64 rendererID, 
		const SizeInt& backbufferSize, 
		std::span<Ref<Image>> backbuffers)
	{
		RenderService& rendering = *RenderService::Get();

		std::vector<RenderTarget> rts = rendering.GetAttachmentCache().GetRenderTargets(pipeline, rendererID, backbufferSize, _sampleCount, backbuffers);
		RenderTarget::SetClearValues(rts, _clearColor, _depthStencilClearValues.X, static_cast<uint8>(_depthStencilClearValues.Y));

		RectInt viewport(Vector2Int::Zero, backbufferSize);
		double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
		Matrix4x4 projection;
		ViewFrustum frustum;

		if (_isPerspective)
		{
			projection = rendering.GetPlatform().CreatePerspectiveProjection(_projectionSize, aspectRatio, _nearClip, _farClip);
			frustum = ViewFrustum::CreatePerspective(
				_viewTransform.GetPosition(TransformSpace::Global),
				_viewTransform.GetForward(TransformSpace::Global),
				_viewTransform.GetUp(TransformSpace::Global),
				_projectionSize,
				aspectRatio,
				_nearClip,
				_farClip);
		}
		else
		{
			projection = rendering.GetPlatform().CreateOrthographicProjection(_projectionSize, aspectRatio, _nearClip, _farClip);
			frustum = ViewFrustum::CreateOrthographic(
				_viewTransform.GetPosition(TransformSpace::Global),
				_viewTransform.GetForward(TransformSpace::Global),
				_viewTransform.GetUp(TransformSpace::Global),
				_projectionSize,
				aspectRatio,
				_nearClip,
				_farClip);
		}

		renderView.Setup(
			rts,
			viewport,
			_viewTransform.GetTransformMatrix(TransformSpace::Global, TransformSpace::Self),
			projection,
			frustum,
			_sampleCount,
			_globalUniformLayoutOverride);
	}

	void BuiltInRenderViewDataProvider::SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip)
	{
		_isPerspective = true;
		_projectionSize = verticalFOVRadians;
		_nearClip = nearClip;
		_farClip = farClip;
	}

	void BuiltInRenderViewDataProvider::SetOrthographicProjection(double verticalOrthographicSize, double nearClip, double farClip)
	{
		_isPerspective = false;
		_projectionSize = verticalOrthographicSize;
		_nearClip = nearClip;
		_farClip = farClip;
	}

	void BuiltInRenderViewDataProvider::SetClearColor(const Color& clearColor)
	{
		_clearColor = clearColor;
	}

	void BuiltInRenderViewDataProvider::SetDepthStencilClearValues(const Vector2& depthStencilClearValue)
	{
		_depthStencilClearValues = depthStencilClearValue;
	}

	void BuiltInRenderViewDataProvider::SetMSAASampleCount(MSAASamples sampleCount)
	{
		_sampleCount = sampleCount;
	}
}