#pragma once

#include <Coco/Core/API.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco::Rendering
{
	struct RenderView;

	/// @brief Types of state changes within a render context
	enum class RenderContextStateChange
	{
		Shader,
		Material
	};

	/// @brief States for a render context to be in
	enum class RenderContextState
	{
		Ready,
		CompilingDrawCalls,
		DrawCallsSubmitted,
	};

	/// @brief An object that holds global uniform data that can be directly pushed to a shader
	struct GlobalUniformObject
	{
		float Projection[4 * 4];	// 64 bytes
		float View[4 * 4];			// 64 bytes
		uint8_t Padding[128];		// 128 bytes - padding

		GlobalUniformObject() noexcept;
		GlobalUniformObject(const RenderView* renderView) noexcept;

	private:
		/// @brief Populates a float array pointer with matrix values
		/// @param destinationMatrixPtr The pointer to the first element of the float array
		/// @param sourceMatrix The matrix
		static void PopulateMatrix(float* destinationMatrixPtr, const Matrix4x4& sourceMatrix) noexcept;
	};
}