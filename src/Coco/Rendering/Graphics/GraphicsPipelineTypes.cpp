#include "Renderpch.h"
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	GraphicsPipelineState::GraphicsPipelineState() :
		TopologyMode(TopologyMode::Triangles),
		CullingMode(CullMode::Back),
		WindingMode(TriangleWindingMode::Clockwise),
		PolygonFillMode(PolygonFillMode::Fill),
		EnableDepthClamping(true),
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
}