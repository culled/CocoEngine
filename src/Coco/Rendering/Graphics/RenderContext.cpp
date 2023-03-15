#include "RenderContext.h"

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
	GlobalUniformObject::GlobalUniformObject() noexcept :
		Projection{0.0f}, View{0.0f}, Padding{0}
	{}

	GlobalUniformObject::GlobalUniformObject(const RenderView* renderView) noexcept :
		Padding{ 0 }
	{
		PopulateMatrix(&Projection[0], renderView->Projection);
		PopulateMatrix(&View[0], renderView->View);
	}

	void GlobalUniformObject::PopulateMatrix(float* destinationMatrixPtr, const Matrix4x4& sourceMatrix) noexcept
	{
		for (int i = 0; i < 16; i++)
		{
			destinationMatrixPtr[i] = static_cast<float>(sourceMatrix.Data[i]);
		}
	}

	bool RenderContext::Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline) noexcept
	{
		if (!IsAvaliableForRendering())
		{
			WaitForRenderingCompleted();
		}

		Reset();

		CurrentPipeline = pipeline;
		RenderView = renderView;
		GlobalUO = GlobalUniformObject(renderView.get());

		return BeginImpl();
	}

	void RenderContext::End() noexcept
	{
		EndImpl();
	}

	void RenderContext::Reset() noexcept
	{
		RenderView.reset();
		CurrentPipeline.reset();
		CurrentRenderPass.reset();
		CurrentRenderPassIndex = 0;

		ResetImpl();
	}

	void RenderContext::RestoreViewport()
	{
		SetViewport(RenderView->RenderOffset, RenderView->RenderSize);
	}

	void RenderContext::SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex) noexcept
	{
		CurrentRenderPass = renderPass;
		CurrentRenderPassIndex = passIndex;
	}
}