#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>

#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
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

	/// <summary>
	/// A graphics device that can perform rendering-related operations
	/// </summary>
	class COCOAPI GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		/// <summary>
		/// Gets the name of this device
		/// </summary>
		/// <returns>This device's name</returns>
		virtual string GetName() const = 0;

		/// <summary>
		/// Gets the type of this device 
		/// </summary>
		/// <returns>This device's type</returns>
		virtual GraphicsDeviceType GetType() const = 0;

		/// <summary>
		/// Gets the version of this device's driver
		/// </summary>
		/// <returns>This device's driver version</returns>
		virtual Version GetDriverVersion() const = 0;

		/// <summary>
		/// Gets the version of this device's API
		/// </summary>
		/// <returns>This device's API version</returns>
		virtual Version GetAPIVersion() const = 0;
	};
}

