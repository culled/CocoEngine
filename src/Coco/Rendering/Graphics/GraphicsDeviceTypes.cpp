#include "Renderpch.h"
#include "GraphicsDeviceTypes.h"

namespace Coco::Rendering
{
	GraphicsDeviceFeatures::GraphicsDeviceFeatures() :
		MaximumMSAASamples(MSAASamples::One),
		SupportsHostVisibleLocalMemory(false),
		MaxImageWidth(0),
		MaxImageHeight(0),
		MaxImageDepth(0),
		MinimumBufferAlignment(0),
		SupportsWireframe(false)
	{}

	GraphicsDeviceCreateParams::GraphicsDeviceCreateParams() :
		PreferredDeviceType(GraphicsDeviceType::Discrete),
		SupportsPresentation(true),
		RequireGraphicsCapability(true),
		RequireTransferCapability(true),
		RequireComputeCapability(true),
		EnableAnisotropicSampling(true),
		EnableDepthClamping(false),
		EnableWireframeDrawing(true)
	{}
}