#pragma once

#include <Coco/Core/API.h>

#include <map>
#include <unordered_map>

namespace Coco
{	
	/// @brief A collection that stores key-value pairs in an ordered fashion
	/// @tparam KeyType 
	/// @tparam ValueType 
	template<typename KeyType, typename ValueType>
	using Map = std::map<KeyType, ValueType>;

	/// @brief A collection that stores key-value pairs in an unordered fashion
	/// @tparam KeyType 
	/// @tparam ValueType 
	template<typename KeyType, typename ValueType>
	using UnorderedMap = std::unordered_map<KeyType, ValueType>;
}