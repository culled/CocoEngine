#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/TimeSpan.h>

namespace Coco::Rendering
{
	/// @brief States for a RenderContext
	enum class RenderContextState
	{
		ReadyForRender,
		InRender,
		EndedRender,
		RenderCompleted
	};

	/// @brief Render stats for a RenderContext
	struct RenderContextRenderStats
	{
		/// @brief The number of triangles drawn
		uint64 TrianglesDrawn;

		/// @brief The number of vertices in the scene
		uint64 VertexCount;

		/// @brief The number of draw calls
		uint64 DrawCalls;

		/// @brief The size of the framebuffer
		SizeInt FramebufferSize;

		/// @brief The amount of time executing the render operation took
		TimeSpan TotalExecutionTime;

		/// @brief The amount of time each render pass took
		std::unordered_map<string, TimeSpan> PassExecutionTime;

		RenderContextRenderStats();

		/// @brief Resets this stats object
		void Reset();
	};
}