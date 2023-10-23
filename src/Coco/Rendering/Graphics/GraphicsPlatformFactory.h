#pragma once

#include <Coco/Core/Types/Refs.h>
#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
	class GraphicsPlatform;

	/// @brief A factory that creates a GraphicsPlatform
	class GraphicsPlatformFactory
	{
	protected:
		GraphicsPlatformCreateParams _createParams;

	public:
		GraphicsPlatformFactory(const GraphicsPlatformCreateParams& createParams);
		virtual ~GraphicsPlatformFactory() = default;

		/// @brief Creates a graphics platform reference
		/// @return The graphics platform
		virtual UniqueRef<GraphicsPlatform> Create() const = 0;

		/// @brief Gets the platform creation parameters this factory was created with
		/// @return The platform creation parameters
		const GraphicsPlatformCreateParams& GetPlatformCreateParameters() const { return _createParams; }
	};
}