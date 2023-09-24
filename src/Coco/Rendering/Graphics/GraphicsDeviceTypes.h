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

		GraphicsDeviceFeatures();
	};
}