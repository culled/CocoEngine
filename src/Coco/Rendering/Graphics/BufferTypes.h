#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of buffer usage
	/// </summary>
	enum class BufferUsageFlags
	{
		None,
		TransferSource,
		TransferDestination,
		Uniform,
		UniformTexel,
		Storage,
		StorageTexel,
		Index,
		Vertex,
		Indirect,
		HostVisible,
	};

	COCOAPI BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b);
	COCOAPI BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b);
	COCOAPI void operator|=(BufferUsageFlags& a, BufferUsageFlags b);
	COCOAPI void operator&=(BufferUsageFlags& a, BufferUsageFlags b);

	COCOAPI bool operator>(BufferUsageFlags a, int b);
}