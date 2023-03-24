#pragma once

#include <Coco/Core/API.h>

#include <optional>

namespace Coco
{
	/// @brief Represents a value that can also be null
	/// @tparam ValueType 
	template<typename ValueType>
	using Optional = std::optional<ValueType>;
}