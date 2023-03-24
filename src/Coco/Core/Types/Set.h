#pragma once

#include <Coco/Core/API.h>

#include <set>

namespace Coco
{
	/// @brief A set of unique items
	/// @tparam ValueType 
	template<typename ValueType>
	using Set = std::set<ValueType>;
}