#pragma once

#include <Coco/Core/API.h>

#include <map>

namespace Coco
{
	/// <summary>
	/// A collection that stores key-value pairs
	/// </summary>
	/// <typeparam name="T">The type of key</typeparam>
	/// <typeparam name="U">The type of value</typeparam>
	template<typename T, typename U>
	using Map = std::map<T, U>;
}