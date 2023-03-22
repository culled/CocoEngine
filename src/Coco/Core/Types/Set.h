#pragma once

#include <Coco/Core/API.h>

#include <set>

namespace Coco
{
	/// <summary>
	/// A set of unique items
	/// </summary>
	/// <typeparam name="T">The type of item</typeparam>
	template<typename T>
	using Set = std::set<T>;
}