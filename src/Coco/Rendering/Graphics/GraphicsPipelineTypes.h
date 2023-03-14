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
}