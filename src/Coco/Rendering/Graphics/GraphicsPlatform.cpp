#include "Renderpch.h"
#include "GraphicsPlatform.h"

namespace Coco::Rendering
{
	GraphicsPlatformCreateParams::GraphicsPlatformCreateParams(const Application& app) :
		App(app),
		DeviceCreateParameters{},
		RenderingExtensions{}
	{}
}
