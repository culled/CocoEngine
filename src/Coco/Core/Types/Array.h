#pragma once

#include <Coco/Core/API.h>

#include <array>

namespace Coco
{
	/// @brief Helper for a constant array of a known length
	/// @tparam ValueType
	/// @tparam Length
	template<typename ValueType, int Length>
	using Array = std::array<ValueType, Length>;
}