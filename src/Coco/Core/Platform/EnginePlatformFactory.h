#pragma once

#include "../Types/Refs.h"
#include "EnginePlatform.h"

namespace Coco
{
	/// @brief A factory for creating an EnginePlatform
	class EnginePlatformFactory
	{
	public:
		/// @brief Creates an EnginePlatform
		/// @return The EnginePlatform
		virtual UniqueRef<EnginePlatform> Create() const = 0;
	};
}