#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of topology that can be rendered
	/// </summary>
	enum class TopologyMode
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip
	};

	/// <summary>
	/// Polygon fill modes
	/// </summary>
	enum class PolygonFillMode
	{
		Point,
		Line,
		Fill
	};

	/// <summary>
	/// Culling modes for geometry
	/// </summary>
	enum class CullMode
	{
		None,
		Back,
		Front,
		Both
	};

	/// <summary>
	/// Depth testing modes
	/// </summary>
	enum class DepthTestingMode
	{
		Never,
		LessThan,
		LessThanOrEqual,
		Equal,
		NotEqual,
		GreaterThan,
		GreaterThanOrEqual,
		Always
	};

	/// <summary>
	/// Factors for blending channels
	/// </summary>
	enum class BlendFactorMode
	{
		Zero,
		One,
		SourceColor,
		OneMinusSourceColor,
		DestinationColor,
		OneMinusDestinationColor,
		SourceAlpha,
		OneMinusSourceAlpha,
		DestinationAlpha,
		OneMinusDestinationAlpha,
	};

	/// <summary>
	/// Operations for blending channels
	/// </summary>
	enum class BlendOperation
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

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