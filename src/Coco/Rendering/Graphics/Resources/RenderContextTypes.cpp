#include "RenderContextTypes.h"

#include "../RenderView.h"

namespace Coco::Rendering
{
	GlobalUniformObject::GlobalUniformObject() noexcept :
		Projection{ 0.0f }, View{ 0.0f }, Padding{ 0 }
	{}

	GlobalUniformObject::GlobalUniformObject(const RenderView* renderView) noexcept :
		Padding{ 0 }
	{
		PopulateMatrix(&Projection[0], renderView->Projection);
		PopulateMatrix(&Projection2D[0], renderView->Projection2D);
		PopulateMatrix(&View[0], renderView->View);
	}

	void GlobalUniformObject::PopulateMatrix(float* destinationMatrixPtr, const Matrix4x4& sourceMatrix) noexcept
	{
		for (int i = 0; i < 16; i++)
		{
			destinationMatrixPtr[i] = static_cast<float>(sourceMatrix.Data[i]);
		}
	}
}