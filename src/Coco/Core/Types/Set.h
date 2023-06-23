#pragma once

#include <Coco/Core/API.h>

#include <set>

namespace Coco
{
	/// @brief A set of unique items
	/// @tparam ValueType 
	template<typename ValueType, class Compare = std::less<ValueType>>
	using Set = std::set<ValueType, Compare>;
}