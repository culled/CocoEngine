#pragma once

#include "GraphicsPipelineTypes.h"

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

	/// @brief Features for a GraphicsDevice
	struct GraphicsDeviceFeatures
	{
		/// @brief The maximum amount of msaa samples this device supports
		MSAASamples MaximumMSAASamples;

		/// @brief If true, this device support host visible local memory
		bool SupportsHostVisibleLocalMemory;

		/// @brief The maximum width of an image
		uint32 MaxImageWidth;

		/// @brief The maximum height of an image
		uint32 MaxImageHeight;

		/// @brief The maximum depth of an image
		uint32 MaxImageDepth;

		/// @brief The minimum alignment for a buffer
		uint32 MinimumBufferAlignment;

		/// @brief The maximum anisotropic filtering level
		uint8 MaxAnisotropicLevel;

		/// @brief If true, this device supports drawing polygons in wireframe
		bool SupportsWireframe;

		GraphicsDeviceFeatures();
	};
	
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

		/// @brief If true, the anisotropic sampling will be enabled on the device if it supports it
		bool EnableAnisotropicSampling;

		/// @brief If true, depth clamping will be enabled on the device if it supports it
		bool EnableDepthClamping;

		/// @brief If true, wireframe drawing will be enabled
		bool EnableWireframeDrawing;

		GraphicsDeviceCreateParams();
	};

}