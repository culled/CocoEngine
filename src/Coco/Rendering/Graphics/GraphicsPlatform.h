#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include "GraphicsDevice.h"
#include "GraphicsPresenter.h"

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

	/// @brief A graphics platform
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;

		/// @brief Gets the name of this platform
		/// @return This platform's name
		virtual const char* GetName() const = 0;

		/// @brief Creates a GraphicsDevice
		/// @param createParams Parameters for creating a GraphicsDevice
		/// @return A GraphicsDevice
		virtual UniqueRef<GraphicsDevice> CreateDevice(const GraphicsDeviceCreateParams& createParams) = 0;

		/// @brief Creates a GraphicsPresenter
		/// @return The created presenter
		virtual UniqueRef<GraphicsPresenter> CreatePresenter() = 0;
	};
}