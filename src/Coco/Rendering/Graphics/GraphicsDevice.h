#pragma once

#include "../Renderpch.h"
#include "GraphicsDeviceTypes.h"
#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering
{
	/// @brief Creation parameters for a GraphicsDevice
	struct GraphicsDeviceCreateParams
	{
		/// @brief The ideal type of rendering device to pick
		GraphicsDeviceType PreferredDeviceType;

		/// @brief If true, the device will be able to present images
		bool SupportsPresentation;

		/// @brief If true, the device will be required to support graphics operations
		bool RequireGraphicsCapability;

		/// @brief If true, the device will be required to support compute operations
		bool RequireComputeCapability;

		/// @brief If true, the device will be required to support memory transfer operations
		bool RequireTransferCapability;

		GraphicsDeviceCreateParams();
	};

	/// @brief A device that can perform graphics operations
	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		/// @brief Gets the name of this device
		/// @return This device's name
		virtual const char* GetName() const = 0;

		/// @brief Gets the type of this device
		/// @return This device's type
		virtual GraphicsDeviceType GetDeviceType() const = 0;

		/// @brief Gets the driver version of this device
		/// @return This device's driver version
		virtual Version GetDriverVersion() const = 0;

		/// @brief Gets the api version of this device
		/// @return This device's api version
		virtual Version GetAPIVersion() const = 0;
	};
}