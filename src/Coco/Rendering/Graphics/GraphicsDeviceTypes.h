#pragma once

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
	struct GraphicsDeviceMemoryFeatures
	{
		/// @brief If true, this device support host visible local memory
		bool SupportsHostVisibleLocalMemory;

		GraphicsDeviceMemoryFeatures();
	};
}