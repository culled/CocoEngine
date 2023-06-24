#pragma once

#include <Coco/Core/API.h>

namespace Coco::ECS
{
	using EntityID = uint64_t;
	static constexpr EntityID InvalidEntityID = Math::MaxValue<EntityID>();
}