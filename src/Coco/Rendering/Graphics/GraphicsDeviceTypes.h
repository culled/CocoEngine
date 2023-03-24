#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief Types of graphics devices
	enum class GraphicsDeviceType
	{
		Discrete,
		Integrated,
		CPU,
		Other
	};

	/// @brief Memory features for a GraphicsDevice
	struct COCOAPI GraphicsDeviceMemoryFeatures
	{
		/// @brief If true, this device support host visible local memory
		bool SupportsLocalHostBufferMemory = false;
	};

	/// @brief Parameters for choosing and creating a graphics device
	struct COCOAPI GraphicsDeviceCreationParameters
	{
		/// @brief The ideal type of rendering device to pick
		GraphicsDeviceType PreferredDeviceType = GraphicsDeviceType::Discrete;

		/// @brief If true, the device will be able to present images
		bool SupportsPresentation = true;

		/// @brief If true, the device will be required to support graphics operations
		bool RequireGraphicsCapability = true;

		/// @brief If true, the device will be required to support compute operations
		bool RequireComputeCapability = true;

		/// @brief If true, the device will be required to support memory transfer operations
		bool RequireTransferCapability = true;
	};
}