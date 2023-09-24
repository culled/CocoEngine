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
		MinimumBufferAlignment(0)
	{}
}