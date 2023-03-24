#pragma once

namespace Coco::Rendering
{
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
}