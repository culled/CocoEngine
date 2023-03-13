#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of buffer usage
	/// </summary>
	enum class BufferUsageFlags
	{
		None				= 0,
		TransferSource		= 1 << 0,
		TransferDestination = 1 << 1,
		Uniform				= 1 << 2,
		UniformTexel		= 1 << 3,
		Storage				= 1 << 4,
		StorageTexel		= 1 << 5,
		Index				= 1 << 6,
		Vertex				= 1 << 7,
		Indirect			= 1 << 8,
		HostVisible			= 1 << 9,
	};

	COCOAPI BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b);
	COCOAPI BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b);
	COCOAPI void operator|=(BufferUsageFlags& a, BufferUsageFlags b);
	COCOAPI void operator&=(BufferUsageFlags& a, BufferUsageFlags b);
}