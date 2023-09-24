#include "Renderpch.h"
#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	GraphicsDeviceCreateParams::GraphicsDeviceCreateParams() :
		PreferredDeviceType(GraphicsDeviceType::Discrete),
		SupportsPresentation(true),
		RequireGraphicsCapability(true),
		RequireTransferCapability(true),
		RequireComputeCapability(true),
		EnableAnisotropicSampling(true),
		EnableDepthClamping(false)
	{}
}