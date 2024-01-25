#pragma once

#include <Coco/Core/Types/Vector.h>

namespace Coco::Physics3D
{
	struct CollisionShape3D
	{
		virtual ~CollisionShape3D() = default;
	};
}