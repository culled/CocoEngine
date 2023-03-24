#pragma once

#include <Coco/Core/API.h>

#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief A state for the graphics pipeline
	struct COCOAPI GraphicsPipelineState
	{
		/// @brief The type of topology that will be rendered
		TopologyMode TopologyMode = TopologyMode::Triangles;

		/// @brief The culling mode for triangles
		CullMode CullingMode = CullMode::Back;

		/// @brief The fill mode for polygons
		PolygonFillMode PolygonFillMode = PolygonFillMode::Fill;

		/// @brief If true, depth values will be clamped to the [0.0, 1.0] range
		bool EnableDepthClamping = false;

		/// @brief The mode for testing fragments against depth
		DepthTestingMode DepthTestingMode = DepthTestingMode::LessThan;

		/// @brief If true, depth values will be written to the depth buffer
		bool EnableDepthWrite = true;
	};
}