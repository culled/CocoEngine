#include "Renderpch.h"
#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
	GraphicsPlatformCreateParams::GraphicsPlatformCreateParams(const Application& app, bool presentationSupport) :
		App(app),
		PresentationSupport(presentationSupport),
		DeviceCreateParameters{},
		RenderingExtensions{}
	{}
}