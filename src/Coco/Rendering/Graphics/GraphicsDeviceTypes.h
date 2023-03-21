#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Types of graphics devices
	/// </summary>
	enum class GraphicsDeviceType
	{
		Discrete,
		Integrated,
		CPU,
		Other
	};

	/// <summary>
	/// Memory features for a GraphicsDevice
	/// </summary>
	struct COCOAPI GraphicsDeviceMemoryFeatures
	{
		/// <summary>
		/// If true, this device support host visible local memory
		/// </summary>
		bool SupportsLocalHostBufferMemory = false;
	};

	/// <summary>
	/// Parameters for choosing and creating a graphics device
	/// </summary>
	struct COCOAPI GraphicsDeviceCreationParameters
	{
		/// <summary>
		/// The ideal type of rendering device to pick
		/// </summary>
		GraphicsDeviceType PreferredDeviceType = GraphicsDeviceType::Discrete;

		/// <summary>
		/// If true, the device will be able to present images
		/// </summary>
		bool SupportsPresentation = true;

		/// <summary>
		/// If true, the device will be required to support graphics operations
		/// </summary>
		bool RequireGraphicsCapability = true;

		/// <summary>
		/// If true, the device will be required to support compute operations
		/// </summary>
		bool RequireComputeCapability = true;

		/// <summary>
		/// If true, the device will be required to support memory transfer operations
		/// </summary>
		bool RequireTransferCapability = true;
	};
}