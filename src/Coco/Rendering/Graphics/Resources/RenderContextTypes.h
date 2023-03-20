#pragma once

namespace Coco::Rendering
{
	/// <summary>
	/// Types of state changes within a render context
	/// </summary>
	enum class RenderContextStateChange
	{
		Shader,
		Material
	};

	/// <summary>
	/// States for a render context to be in
	/// </summary>
	enum class RenderContextState
	{
		Ready,
		CompilingDrawCalls,
		DrawCallsSubmitted,
	};
}