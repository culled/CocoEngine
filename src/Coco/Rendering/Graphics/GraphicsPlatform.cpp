#include "Renderpch.h"
#include "GraphicsPlatform.h"

namespace Coco::Rendering
{
	GraphicsPlatformCreateParams::GraphicsPlatformCreateParams(const Application& app, bool presentationSupport) :
		App(app),
		PresentationSupport(presentationSupport),
		DeviceCreateParameters{},
		RenderingExtensions{}
	{}
}
