#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief A generic component that can be added to entities
	class COCOAPI EntityComponent
	{
	public:
		virtual ~EntityComponent() = default;
	};
}