#pragma once

#include <Coco/Vendor/uuid_v4/uuid_v4.h>

namespace Coco
{
	using UUID = UUIDv4::UUID;

	/// @brief Creates a new UUID
	/// @return A UUID
	UUID CreateUUIDv4();
}