#pragma once

#include <Coco/Core/API.h>

#include <map>

namespace Coco
{	
	/// @brief A collection that stores key-value pairs
	/// @tparam KeyType 
	/// @tparam ValueType 
	template<typename KeyType, typename ValueType>
	using Map = std::map<KeyType, ValueType>;
}