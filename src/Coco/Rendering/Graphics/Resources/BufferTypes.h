#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief Types of buffer usage
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

	COCOAPI BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) noexcept;
	COCOAPI BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b) noexcept;
	COCOAPI void operator|=(BufferUsageFlags& a, BufferUsageFlags b) noexcept;
	COCOAPI void operator&=(BufferUsageFlags& a, BufferUsageFlags b) noexcept;

	/// @brief Data types that a buffer can hold and shaders can read
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
		Color,
		Matrix4x4,
	};

	/// @brief Gets the size of a BufferDataFormat
	/// @param format The buffer data format
	/// @return The size of the buffer data format (in bytes)
	constexpr uint GetBufferDataFormatSize(BufferDataFormat format) noexcept
	{
		switch (format)
		{
		case BufferDataFormat::Float:
		case BufferDataFormat::Int:
			return 4;
		case BufferDataFormat::Vector2:
		case BufferDataFormat::Vector2Int:
			return 4 * 2;
		case BufferDataFormat::Vector3:
		case BufferDataFormat::Vector3Int:
			return 4 * 3;
		case BufferDataFormat::Vector4:
		case BufferDataFormat::Vector4Int:
		case BufferDataFormat::Color:
			return 4 * 4;
		case BufferDataFormat::Matrix4x4:
			return 4 * 4 * 4;
		default:
			return 0;
		}
	}
}