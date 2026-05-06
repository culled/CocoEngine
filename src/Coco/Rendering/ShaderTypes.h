//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_SHADERTYPES_H
#define COCOENGINE_SHADERTYPES_H
#include <Coco/Core/Types/CoreTypes.h>

#include "Coco/Core/IO/FilePath.h"
#include "Coco/Core/Types/EnumTypes.h"
#include "Coco/Core/Types/String.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco
{
    /// @brief Types of topology that can be rendered
	enum class MeshTopologyMode : uint8
	{
		/// @brief Draw points
		Points = 0,

		/// @brief Draw lines in a list
		Lines,

		/// @brief Draw lines in a continuous strip
		LineStrip,

		/// @brief Draw triangles in a list
		Triangles,

		/// @brief Draw triangles in a continuous strip
		TriangleStrip
	};

	/// @brief Polygon fill modes
	enum class PolygonFillMode : uint8
	{
		/// @brief Draw vertices as points
		Point = 0,

		/// @brief Draw lines between points
		Line,

		/// @brief Fill triangles
		Fill
	};

	/// @brief Culling modes for geometry
	enum class CullMode : uint8
	{
		/// @brief No culling
		None = 0,

		/// @brief Triangle back-faces are culled
		Back,

		/// @brief Triangle front-faces are culled
		Front,

		/// @brief Both faces are culled, essentially culling the triangle
		Both
	};

	/// @brief Triangle winding modes for geometry
	enum class TriangleWindingMode : uint8
	{
		/// @brief Clockwise screen winding corresponds to a front-facing triangle
		Clockwise = 0,

		/// @brief Counter-clockwise screen winding corresponds to a front-facing triangle
		CounterClockwise
	};

	/// @brief Depth testing modes
	enum class DepthTestingMode : uint8
	{
		/// @brief Depth testing is not performed and always passes
		Never = 0,

		/// @brief Depth less than the stored depth passes
		LessThan,

		/// @brief Depth less or equal to the stored depth passes
		LessThanOrEqual,

		/// @brief Depth equal to the stored depth passes
		Equal,

		/// @brief Depth not equal to the stored depth passes
		NotEqual,

		/// @brief Depth greater than the stored depth passes
		GreaterThan,

		/// @brief Depth greater than or equal to the stored depth passes
		GreaterThanOrEqual,

		/// @brief Only unwritten depth passes, and fails against any previously-written depth
		Always
	};

	/// @brief Factors for blending channels
	enum class BlendFactorMode : uint8
	{
		/// @brief Factor is a constant zero
		Zero = 0,

		/// @brief Factor is a constant one
		One,

		/// @brief Factor is the source color values
		SourceColor,

		/// @brief Factor is one minus the source color values
		OneMinusSourceColor,

		/// @brief Factor is the destination color values
		DestinationColor,

		/// @brief Factor is one minus the destination color values
		OneMinusDestinationColor,

		/// @brief Factor is the source alpha value
		SourceAlpha,

		/// @brief Factor is one minus the source alpha value
		OneMinusSourceAlpha,

		/// @brief Factor is the destination alpha value
		DestinationAlpha,

		/// @brief Factor is one minus the destination alpha value
		OneMinusDestinationAlpha,
	};

	/// @brief Operations for blending channels
	enum class BlendOperation : uint8
	{
		/// @brief No blending is performed
		None = 0,

		/// @brief Source values are added to destination values
		Add,

		/// @brief Source values are subtracted from destination values
		Subtract,

		/// @brief Desination values are subtracted from source values
		ReverseSubtract,

		/// @brief The minimum of the source and destination values
		Min,

		/// @brief The maximum of the source and destination values
		Max
	};

	/// @brief A blend state for a single attachment
	struct AttachmentBlendState
	{
		/// @brief A blend state with no blending
		static const AttachmentBlendState None;

		/// @brief A blend state that can be used for opaque operations
		static const AttachmentBlendState Opaque;

		/// @brief A blend state that blends based on source alpha
		static const AttachmentBlendState AlphaBlending;

		/// @brief The blend factor for the color source
		BlendFactorMode ColorSourceFactor;

		/// @brief The blend factor for the color destination
		BlendFactorMode ColorDestinationFactor;

		/// @brief The operation to use for combining the source and destination colors
		BlendOperation ColorBlendOperation;

		/// @brief The blend factor for the alpha source
		BlendFactorMode AlphaSourceFactor;

		/// @brief The blend factor for the alpha destination
		BlendFactorMode AlphaDestinationFactor;

		/// @brief The operation to use for combining the source and destination alphas
		BlendOperation AlphaBlendOperation;

		AttachmentBlendState(
			BlendFactorMode colorSrcFactor,
			BlendFactorMode colorDstFactor,
			BlendOperation colorOp,
			BlendFactorMode alphaSrcFactor,
			BlendFactorMode alphaDstFactor,
			BlendOperation alphaOp) noexcept;

		bool operator==(const AttachmentBlendState& rhs) const noexcept;
	};

	uint64 ToHash(const AttachmentBlendState& blendState);

	/// @brief A state for the graphics pipeline
	struct GraphicsPipelineState
	{
		/// @brief The type of topology that will be rendered
		MeshTopologyMode TopologyMode;

		/// @brief The culling mode for triangles
		CullMode CullingMode;

		/// @brief The winding mode for triangles
		TriangleWindingMode WindingMode;

		/// @brief The fill mode for polygons
		PolygonFillMode FillMode;

		/// @brief If true, depth values will be clamped to the [0.0, 1.0] range
		bool EnableDepthClamping;

		/// @brief The mode for testing fragments against depth
		DepthTestingMode DepthTestMode;

		/// @brief If true, depth values will be written to the depth buffer
		bool EnableDepthWrite;

		AttachmentBlendState BlendState;

		GraphicsPipelineState() noexcept;

		bool operator==(const GraphicsPipelineState& rhs) const noexcept;
	};

	uint64 ToHash(const GraphicsPipelineState& state);

	/// @brief Types of shader stages
	enum class ShaderStageType : uint8
	{
		Vertex = 0,
		Fragment,
		Compute,
		Tesselation,
		Geometry
	};

	/// @brief Types of shader stages
	enum class ShaderStageFlags : uint8
	{
		None = 0,
		Vertex = 1 << 0,
		Fragment = 1 << 1,
		Compute = 1 << 2,
		Tesselation = 1 << 3,
		Geometry = 1 << 4,
		All = (1 << 5) - 1
	};

	EnumFlagOperators(ShaderStageFlags)
}
#endif //COCOENGINE_SHADERTYPES_H