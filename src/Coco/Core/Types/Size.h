#pragma once

#include "String.h"

namespace Coco
{
	/// @brief Template for sizes
	/// @tparam ValueType The type of value to use
	template<typename ValueType>
	struct SizeBase
	{
		/// @brief The width
		ValueType Width;

		/// @brief The height
		ValueType Height;

		SizeBase() :
			Width{},
			Height{}
		{}

		SizeBase(ValueType width, ValueType height) :
			Width(width),
			Height(height)
		{}

		virtual ~SizeBase() = default;

		/// @brief Gets a string representation of this size
		/// @return The string representation
		string ToString() const
		{
			return FormatString("{}x{}", Width, Height);
		}
	};

	/// @brief A size that uses uint32 values
	struct SizeInt : SizeBase<uint32>
	{
		/// @brief A size with zero width and height
		static const SizeInt Zero;

		SizeInt() = default;
		SizeInt(uint32 width, uint32 height);
	};
}