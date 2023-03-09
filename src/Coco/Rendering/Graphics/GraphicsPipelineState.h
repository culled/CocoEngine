#pragma once

#include <Coco/Core/Core.h>

#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A state for the graphics pipeline
	/// </summary>
	struct COCOAPI GraphicsPipelineState
	{
		/// <summary>
		/// The type of topology that will be rendered
		/// </summary>
		TopologyMode TopologyMode = TopologyMode::Triangles;

		/// <summary>
		/// The culling mode for triangles
		/// </summary>
		CullMode CullingMode = CullMode::Back;

		/// <summary>
		/// The fill mode for polygons
		/// </summary>
		PolygonFillMode PolygonFillMode = PolygonFillMode::Fill;

		/// <summary>
		/// If true, depth values will be clamped to the [0.0, 1.0] range
		/// </summary>
		bool EnableDepthClamping = false;

		/// <summary>
		/// The mode for testing fragments against depth
		/// </summary>
		DepthTestingMode DepthTestingMode = DepthTestingMode::LessThan;

		/// <summary>
		/// If true, depth values will be written to the depth buffer
		/// </summary>
		bool EnableDepthWrite = true;
	};
}