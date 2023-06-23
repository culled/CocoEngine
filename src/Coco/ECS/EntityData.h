#pragma once

#include <Coco/Core/Core.h>
#include <bitset>

namespace Coco::ECS
{
	using EntityID = uint64_t;

	constexpr EntityID InvalidEntityID = std::numeric_limits<EntityID>::max();

	struct EntityData
	{
		const EntityID ID;
		string Name;

		EntityData(EntityID id, string name = "");
	};
}