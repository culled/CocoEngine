#include "RenderContext.h"

#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
	GlobalUniformObject::GlobalUniformObject()
	{
		std::memset(Projection, 0, 16 * sizeof(float));
		std::memset(View, 0, 16 * sizeof(float));
		std::memset(Padding, 0, 128 * sizeof(uint8_t));
	}

	GlobalUniformObject::GlobalUniformObject(const Ref<RenderView>& renderView)
	{
		PopulateMatrix(Projection, renderView->Projection);
		PopulateMatrix(View, renderView->View);

		std::memset(Padding, 0, 128 * sizeof(uint8_t));
	}

	void GlobalUniformObject::PopulateMatrix(float* destinationMatrixPtr, const Matrix4x4& sourceMatrix)
	{
		const double* sourceMatrixPtr = &sourceMatrix.Data[0];

		for (int i = 0; i < 16; i++)
		{
			destinationMatrixPtr[i] = static_cast<float>(*sourceMatrixPtr);
			sourceMatrixPtr++;
		}
	}

	bool RenderContext::Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline>& pipeline)
	{
		if (!IsAvaliableForRendering())
		{
			WaitForRenderingCompleted();
		}

		Reset();

		CurrentPipeline = pipeline;
		RenderView = renderView;
		GlobalUO = GlobalUniformObject(renderView);

		return BeginImpl();
	}

	void RenderContext::End()
	{
		EndImpl();
	}

	void RenderContext::Reset()
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

	void RenderContext::SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex)
	{
		CurrentRenderPass = renderPass;
		CurrentRenderPassIndex = passIndex;
	}
}