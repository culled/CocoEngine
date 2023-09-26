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

		/// @brief The number of vertices drawn
		uint64 VertexCount;

		/// @brief The number of draw calls
		uint64 DrawCalls;

		/// @brief The amount of time executing render pipelines
		TimeSpan TotalExecutionTime;

		RenderStats();

		void operator+=(const RenderContextRenderStats& other);

		/// @brief Resets this stats object
		void Reset();
	};
}