#include "Renderpch.h"
#include "RenderStats.h"

namespace Coco::Rendering
{
	RenderStats::RenderStats() :
		TrianglesDrawn(0),
		VertexCount(0),
		PipelineExecutionTime{},
		PassExecutionTime{}
	{}

	void RenderStats::operator+=(const RenderContextRenderStats& other)
	{
		TrianglesDrawn += other.TrianglesDrawn;
		VertexCount += other.VertexCount;
	}

	void RenderStats::Reset()
	{
		TrianglesDrawn = 0;
		VertexCount = 0;
		PipelineExecutionTime = TimeSpan();
		PassExecutionTime.clear();
	}
}