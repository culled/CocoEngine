#pragma once

#include <Coco/Core/Defines.h>

namespace Coco::Rendering
{
	/// @brief Size of a float in bytes
	constexpr uint8 BufferFloatSize = 4;

	/// @brief Size of an int in bytes
	constexpr uint8 BufferIntSize = 4;

	/// @brief Type of data that can be stored in a buffer
	enum class BufferDataType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Mat4x4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	/// @brief Gets the size of the given type of data, in bytes
	/// @param type The data type
	/// @return The size, in bytes
	uint8 GetDataTypeSize(BufferDataType type);

	/// @brief Gets the number of components in a type of data
	/// @param type The data type
	/// @return The number of individual components that compose the type
	uint8 GetDataTypeComponentCount(BufferDataType type);

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

	constexpr BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) { return static_cast<BufferUsageFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	constexpr BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b) { return static_cast<BufferUsageFlags>(static_cast<int>(a) & static_cast<int>(b)); }

	constexpr void operator|=(BufferUsageFlags& a, BufferUsageFlags b) { a = a | b; }
	constexpr void operator&=(BufferUsageFlags& a, BufferUsageFlags b) { a = a & b; }
}