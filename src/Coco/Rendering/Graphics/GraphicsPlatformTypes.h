#pragma once

#include "../Renderpch.h"
#include "GraphicsDeviceTypes.h"

namespace Coco
{
	class Application;
}

namespace Coco::Rendering
{
	/// @brief Creation parameters for a GraphicsPlatform
	struct GraphicsPlatformCreateParams
	{
		/// @brief The application instance.
		/// This is here because if the app creates the RenderService in its constructor, the Engine's app instance is technically not set yet and not accessible
		const Application& App;

		/// @brief Set to true to enable the platform to present rendered images
		bool PresentationSupport;

		/// @brief Parameters for creating the GraphicsDevice
		GraphicsDeviceCreateParams DeviceCreateParameters;

		/// @brief Platform rendering extensions to enable
		std::vector<const char*> RenderingExtensions;

		GraphicsPlatformCreateParams(const Application& app, bool presentationSupport);
	};

}