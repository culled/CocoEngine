#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Vertex data that can be sent to the GPU
	/// </summary>
	struct COCOAPI VertexData
	{
		float Position[3];
	};
}