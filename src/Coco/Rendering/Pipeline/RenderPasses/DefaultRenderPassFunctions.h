#pragma once

namespace Coco::Rendering
{
	class RenderContext;
	class RenderView;

	/// @brief Common default functionality for built-in RenderPasses
	struct DefaultRenderPassFunctions
	{
		/// @brief Applies the global uniforms according to the default GlobalShaderUniformLayout in the RenderView
		/// @param context The render context
		/// @param renderView The render view
		static void ApplyDefaultPreparations(RenderContext& context, const RenderView& renderView);
	};
}