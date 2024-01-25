#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Physics3DPlatform.h"

namespace Coco::Physics3D
{
	class Physics3DPlatformFactory
	{
	public:
		virtual ~Physics3DPlatformFactory() = default;

		virtual UniqueRef<Physics3DPlatform> Create() const = 0;
	};
}