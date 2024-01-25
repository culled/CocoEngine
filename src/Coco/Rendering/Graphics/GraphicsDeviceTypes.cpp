#include "Renderpch.h"
#include "GraphicsDeviceTypes.h"

namespace Coco::Rendering
{
	GraphicsDeviceFeatures::GraphicsDeviceFeatures() :
		Name(""),
		Type(GraphicsDeviceType::Other),
		DriverVersion(),
		MaxImageWidth(0),
		MaxImageHeight(0),
		MaxImageDepth(0),
		MinimumBufferAlignment(0),
		MaxAnisotropicLevel(0),
		SupportsWireframe(false)
	{}
}