#include "Renderpch.h"
#include "RenderContextTypes.h"

namespace Coco::Rendering
{
	RenderContextRenderStats::RenderContextRenderStats() :
		TrianglesDrawn(0),
		VertexCount(0),
		DrawCalls(0),
		FramebufferSize(SizeInt::Zero),
		TotalExecutionTime(),
		PassExecutionTime()
	{}

	void RenderContextRenderStats::Reset()
	{
		TrianglesDrawn = 0;
		VertexCount = 0;
		DrawCalls = 0;
		FramebufferSize = SizeInt::Zero;
		TotalExecutionTime = TimeSpan();
		PassExecutionTime.clear();
	}
}