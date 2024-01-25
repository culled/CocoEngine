#pragma once

#include <Coco/Core/Types/Refs.h>
#include "GraphicsPlatform.h"

namespace Coco::Rendering
{
	/// @brief A factory that creates the GraphicsPlatform
	class GraphicsPlatformFactory
	{
	public:
		virtual ~GraphicsPlatformFactory() = default;

		/// @brief Creates a GraphicsPlatform
		/// @return The GraphicsPlatform
		virtual UniqueRef<GraphicsPlatform> Create() const = 0;
	};
}