#include "Renderpch.h"
#include "RenderStats.h"

namespace Coco::Rendering
{
	RenderStats::RenderStats() :
		TrianglesDrawn(0),
		VertexCount(0),
		DrawCalls(0),
		TotalExecutionTime()
	{}

	void RenderStats::operator+=(const RenderContextRenderStats& other)
	{
		TrianglesDrawn += other.TrianglesDrawn;
		VertexCount += other.VertexCount;
		DrawCalls += other.DrawCalls;
		TotalExecutionTime += other.TotalExecutionTime;
	}

	void RenderStats::Reset()
	{
		TrianglesDrawn = 0;
		VertexCount = 0;
		DrawCalls = 0;
		TotalExecutionTime = TimeSpan();
		RenderSyncWait = TimeSpan();
	}
}