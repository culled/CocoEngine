#include "Renderpch.h"
#include "GraphicsPipelineTypes.h"
#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	const BlendState BlendState::Opaque = BlendState(
		BlendFactorMode::One, 
		BlendFactorMode::Zero, 
		BlendOperation::None, 
		BlendFactorMode::One, 
		BlendFactorMode::One, 
		BlendOperation::None);

	const BlendState BlendState::AlphaBlending = BlendState(
		BlendFactorMode::SourceAlpha,
		BlendFactorMode::OneMinusSourceAlpha,
		BlendOperation::Add,
		BlendFactorMode::SourceAlpha,
		BlendFactorMode::OneMinusSourceAlpha,
		BlendOperation::Add);

	BlendState::BlendState(
		BlendFactorMode colorSrcFactor, 
		BlendFactorMode colorDstFactor, 
		BlendOperation colorOp, 
		BlendFactorMode alphaSrcFactor, 
		BlendFactorMode alphaDstFactor, 
		BlendOperation alphaOp) :
		ColorSourceFactor(colorSrcFactor),
		ColorDestinationFactor(colorDstFactor),
		ColorBlendOperation(colorOp),
		AlphaSourceFactor(alphaSrcFactor),
		AlphaDestinationFactor(alphaDstFactor),
		AlphaBlendOperation(alphaOp)
	{}

	bool BlendState::operator==(const BlendState& other) const
	{
		return ColorSourceFactor == other.ColorSourceFactor &&
			ColorDestinationFactor == other.ColorDestinationFactor &&
			ColorBlendOperation == other.ColorBlendOperation &&
			AlphaSourceFactor == other.AlphaSourceFactor &&
			AlphaDestinationFactor == other.AlphaDestinationFactor &&
			AlphaBlendOperation == other.AlphaBlendOperation;
	}

	uint64 BlendState::GetHash() const
	{
		return Math::CombineHashes(
			static_cast<uint64>(ColorSourceFactor),
			static_cast<uint64>(ColorDestinationFactor),
			static_cast<uint64>(ColorBlendOperation),
			static_cast<uint64>(AlphaSourceFactor),
			static_cast<uint64>(AlphaDestinationFactor),
			static_cast<uint64>(AlphaBlendOperation));
	}

	GraphicsPipelineState::GraphicsPipelineState() :
		TopologyMode(TopologyMode::Triangles),
		CullingMode(CullMode::Back),
		WindingMode(TriangleWindingMode::Clockwise),
		PolygonFillMode(PolygonFillMode::Fill),
		EnableDepthClamping(false),
		DepthTestingMode(DepthTestingMode::LessThanOrEqual),
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