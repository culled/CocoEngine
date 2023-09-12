#include "Renderpch.h"
#include "RenderService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory)
	{
		_platform = platformFactory.Create();
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}
}