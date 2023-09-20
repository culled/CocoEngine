#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/TimeSpan.h>
#include <Coco/Core/Types/String.h>
#include "Graphics/RenderContext.h"

namespace Coco::Rendering
{
	/// @brief Statistics for a render operation
	struct RenderStats
	{
		/// @brief The number of triangles drawn
		uint64 TrianglesDrawn;

		/// @brief The number of vertices in the scene
		uint64 VertexCount;

		/// @brief The amount of time executing the pipeline took
		TimeSpan PipelineExecutionTime;

		/// @brief The amount of time each render pass took
		std::unordered_map<string, TimeSpan> PassExecutionTime;

		RenderStats();

		void operator+=(const RenderContextRenderStats& other);

		/// @brief Resets this stats object
		void Reset();
	};
}