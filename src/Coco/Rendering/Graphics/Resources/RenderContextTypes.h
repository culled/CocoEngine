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
		Material,
		Uniform,
		Texture
	};

	/// @brief States for a render context to be in
	enum class RenderContextState
	{
		Ready,
		CompilingDrawCalls,
		DrawCallsSubmitted,
	};
}