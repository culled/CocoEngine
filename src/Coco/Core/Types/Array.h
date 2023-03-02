#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Helper for a constant array of N length
	/// </summary>
	/// <typeparam name="T">The type of value for the array</typeparam>
	template<typename T, int N>
	using Array = std::array<T, N>;
}