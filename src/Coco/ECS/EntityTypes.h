#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Math/Math.h>

namespace Coco::ECS
{
	/// @brief An ID for an entity
	using EntityID = uint64;

	/// @brief An invalid entity ID
	const EntityID InvalidEntityID = Math::MaxValue<uint64>();
}