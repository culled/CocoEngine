#pragma once

#include <Coco/Core/API.h>

#include <array>

namespace Coco
{
	/// <summary>
	/// Helper for a constant array of N length
	/// </summary>
	/// <typeparam name="ArrayT">The type of value for the array</typeparam>
	template<typename ArrayT, int Length>
	using Array = std::array<ArrayT, Length>;
}