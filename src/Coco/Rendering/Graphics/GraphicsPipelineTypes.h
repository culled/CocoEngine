#pragma once

#include <Coco/Core/Defines.h>

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

	/// @brief Triangle winding modes for geometry
	enum class TriangleWindingMode
	{
		Clockwise,
		CounterClockwise
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

	/// @brief Options for MSAA samples
	enum class MSAASamples
	{
		One,
		Two,
		Four,
		Eight,
		Sixteen
	};

	/// @brief A blend state for a single attachment
	struct BlendState
	{
		/// @brief A blend state that can be used for opaque operations
		static const BlendState Opaque;
		
		/// @brief A blend state that blends based on source alpha
		static const BlendState AlphaBlending;

		/// @brief The blend factor for the color source
		BlendFactorMode ColorSourceFactor;

		/// @brief The blend factor for the color destination
		BlendFactorMode ColorDestinationFactor;

		/// @brief The operation to use for combining the source and destination colors
		BlendOperation ColorBlendOperation;

		/// @brief The blend factor for the alpha source
		BlendFactorMode AlphaSourceBlendFactor;

		/// @brief The blend factor for the alpha destination
		BlendFactorMode AlphaDestinationBlendFactor;

		/// @brief The operation to use for combining the source and destination alphas
		BlendOperation AlphaBlendOperation;

		BlendState(
			BlendFactorMode colorSrcFactor,
			BlendFactorMode colorDstFactor,
			BlendOperation colorOp,
			BlendFactorMode alphaSrcFactor,
			BlendFactorMode alphaDstFactor,
			BlendOperation alphaOp);

		bool operator==(const BlendState& other) const;

		/// @brief Gets the hash of this blend state
		/// @return The hash
		uint64 GetHash() const;
	};

	/// @brief A state for the graphics pipeline
	struct GraphicsPipelineState
	{
		/// @brief The type of topology that will be rendered
		TopologyMode TopologyMode;

		/// @brief The culling mode for triangles
		CullMode CullingMode;

		/// @brief The winding mode for triangles
		TriangleWindingMode WindingMode;

		/// @brief The fill mode for polygons
		PolygonFillMode PolygonFillMode;

		/// @brief If true, depth values will be clamped to the [0.0, 1.0] range
		bool EnableDepthClamping;

		/// @brief The mode for testing fragments against depth
		DepthTestingMode DepthTestingMode;

		/// @brief If true, depth values will be written to the depth buffer
		bool EnableDepthWrite;

		GraphicsPipelineState();

		bool operator==(const GraphicsPipelineState& other) const;

		/// @brief Gets the hash of this pipeline state
		/// @return The hash
		uint64 GetHash() const;
	};
}