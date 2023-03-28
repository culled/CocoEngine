#pragma once

namespace Coco::Rendering
{
	/// @brief Types of topology that can be rendered
	enum class TopologyMode
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip
	};

	/// @brief Polygon fill modes
	enum class PolygonFillMode
	{
		Point,
		Line,
		Fill
	};

	/// @brief Culling modes for geometry
	enum class CullMode
	{
		None,
		Back,
		Front,
		Both
	};

	/// @brief Depth testing modes
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

	/// @brief Factors for blending channels
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

	/// @brief Operations for blending channels
	enum class BlendOperation
	{
		None,
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};
}