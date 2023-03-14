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

	/// <summary>
	/// Data types that a buffer can hold and shaders can read
	/// </summary>
	enum class BufferDataFormat
	{
		Float,
		Int,
		Vector2,
		Vector2Int,
		Vector3,
		Vector3Int,
		Vector4,
		Vector4Int,
	};

	/// <summary>
	/// Gets the size of a BufferDataFormat
	/// </summary>
	/// <param name="format">The buffer data format</param>
	/// <returns>The size of the buffer data format (in bytes)</returns>
	COCOAPI uint32_t GetBufferDataFormatSize(BufferDataFormat format);
}