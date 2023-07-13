#pragma once

#include <Coco/Core/API.h>
#include <Coco/Core/Types/UUID.h>

namespace Coco::ECS
{
	using EntityID = UUID;
	const EntityID InvalidEntityID = EntityID("00000000-0000-0000-0000-000000000000");
}