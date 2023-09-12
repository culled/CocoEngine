#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Core.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsDevice.h"

namespace Coco::Rendering
{
	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService
	{
	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory);
		~RenderService();
	};
}