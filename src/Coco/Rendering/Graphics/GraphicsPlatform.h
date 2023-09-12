#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include "GraphicsDevice.h"

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

		/// @brief Parameters for creating the GraphicsDevice
		GraphicsDeviceCreateParams DeviceCreateParameters;

		/// @brief Platform rendering extensions to enable
		std::vector<const char*> RenderingExtensions;

		GraphicsPlatformCreateParams(const Application& app);
	};

	/// @brief A graphics platform
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;

		/// @brief Creates a GraphicsDevice
		/// @param createParams Parameters for creating a GraphicsDevice
		/// @return A GraphicsDevice
		virtual UniqueRef<GraphicsDevice> CreateDevice(const GraphicsDeviceCreateParams& createParams) = 0;
	};
}