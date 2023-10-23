#pragma once
#include "../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Factory for creating a RenderPipeline with a BuiltInRenderPass
	struct BuiltInPipeline
	{
		/// @brief Creates a built-in render pipeline
		/// @param useFrustumCulling If true, the built-in render pass will use frustum culling
		/// @return The created pipeline
		static SharedRef<RenderPipeline> Create(bool useFrustumCulling);
	};
}