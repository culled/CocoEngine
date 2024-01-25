#pragma once
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Version.h>
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief Types of graphics devices
	enum class GraphicsDeviceType
	{
		/// @brief A discrete GPU
		Discrete,

		/// @brief An integrated GPU
		Integrated,

		/// @brief The CPU
		CPU,

		/// @brief A different type of GPU
		Other
	};

	/// @brief Features for a GraphicsDevice
	struct GraphicsDeviceFeatures
	{
		string Name;
		GraphicsDeviceType Type;
		Version DriverVersion;

		/// @brief The maximum amount of msaa samples this device supports
		MSAASamples MaximumMSAASamples;

		/// @brief The maximum width of an image
		uint32 MaxImageWidth;

		/// @brief The maximum height of an image
		uint32 MaxImageHeight;

		/// @brief The maximum depth of an image
		uint32 MaxImageDepth;

		/// @brief The minimum alignment for a buffer
		uint32 MinimumBufferAlignment;

		/// @brief The maximum anisotropic filtering level
		uint32 MaxAnisotropicLevel;

		/// @brief If true, this device supports drawing polygons in wireframe
		bool SupportsWireframe;

		GraphicsDeviceFeatures();
	};
}

