#include "Renderpch.h"
#include "GraphicsPipelineTypes.h"
#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	BlendState::BlendState() :
		ColorSourceFactor(BlendFactorMode::One),
		ColorDestinationFactor(BlendFactorMode::Zero),
		ColorBlendOperation(BlendOperation::None),
		AlphaSourceBlendFactor(BlendFactorMode::One),
		AlphaDestinationBlendFactor(BlendFactorMode::One),
		AlphaBlendOperation(BlendOperation::None)
	{}

	bool BlendState::operator==(const BlendState& other) const
	{
		return ColorSourceFactor == other.ColorSourceFactor &&
			ColorDestinationFactor == other.ColorDestinationFactor &&
			ColorBlendOperation == other.ColorBlendOperation &&
			AlphaSourceBlendFactor == other.AlphaSourceBlendFactor &&
			AlphaDestinationBlendFactor == other.AlphaDestinationBlendFactor &&
			AlphaBlendOperation == other.AlphaBlendOperation;
	}

	uint64 BlendState::GetHash() const
	{
		return Math::CombineHashes(
			static_cast<uint64>(ColorSourceFactor),
			static_cast<uint64>(ColorDestinationFactor),
			static_cast<uint64>(ColorBlendOperation),
			static_cast<uint64>(AlphaSourceBlendFactor),
			static_cast<uint64>(AlphaDestinationBlendFactor),
			static_cast<uint64>(AlphaBlendOperation));
	}

	GraphicsPipelineState::GraphicsPipelineState() :
		TopologyMode(TopologyMode::Triangles),
		CullingMode(CullMode::Back),
		WindingMode(TriangleWindingMode::Clockwise),
		PolygonFillMode(PolygonFillMode::Fill),
		EnableDepthClamping(false),
		DepthTestingMode(DepthTestingMode::LessThan),
		EnableDepthWrite(true)
	{}

	bool GraphicsPipelineState::operator==(const GraphicsPipelineState& other) const
	{
		return TopologyMode == other.TopologyMode &&
			CullingMode == other.CullingMode &&
			WindingMode == other.WindingMode &&
			PolygonFillMode == other.PolygonFillMode &&
			EnableDepthClamping == other.EnableDepthClamping &&
			DepthTestingMode == other.DepthTestingMode &&
			EnableDepthWrite == other.EnableDepthWrite;
	}

	uint64 GraphicsPipelineState::GetHash() const
	{
		return Math::CombineHashes(
			static_cast<uint64>(TopologyMode),
			static_cast<uint64>(CullingMode),
			static_cast<uint64>(WindingMode),
			static_cast<uint64>(PolygonFillMode),
			static_cast<uint64>(EnableDepthClamping),
			static_cast<uint64>(DepthTestingMode),
			static_cast<uint64>(EnableDepthWrite));
	}
}