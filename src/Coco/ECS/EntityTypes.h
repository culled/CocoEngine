#pragma once

#include <Coco/Core/API.h>
#include <Coco/Core/Types/UUID.h>

namespace Coco::ECS
{
	/// @brief An ID for an entity
	using EntityID = UUID;

	/// @brief An invalid entity ID
	const EntityID InvalidEntityID = UUID::Nil;
}