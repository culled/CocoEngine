#pragma once

#include <Coco/Core/API.h>

#include <optional>

namespace Coco
{
	/// <summary>
	/// Represents a value that can also be null
	/// </summary>
	/// <typeparam name="T">The type of the value</typeparam>
	template<typename T>
	using Optional = std::optional<T>;
}