#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Core.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsDevice.h"

namespace Coco::Rendering
{
	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService, public Singleton<RenderService>
	{
	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory);
		~RenderService();

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		GraphicsPlatform* GetPlatform() { return _platform.get(); }

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		const GraphicsPlatform* GetPlatform() const { return _platform.get(); }

		/// @brief Gets the graphics device
		/// @return The graphics device
		GraphicsDevice* GetDevice() { return _device.get(); }

		/// @brief Gets the graphics device
		/// @return The graphics device
		const GraphicsDevice* GetDevice() const { return _device.get(); }
	};
}