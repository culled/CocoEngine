#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Windowing
{
	/// @brief Information for a display
	struct DisplayInfo
	{
		/// @brief The name of the display
		string Name;

		/// @brief The offset of the top-left of the display in virtual screen coordinates
		Vector2Int Offset;

		/// @brief The resolution of the display
		SizeInt Resolution;

		/// @brief If true, this is the primary display for the system
		bool IsPrimary;

		DisplayInfo(const string& name, const Vector2Int& offset, const SizeInt& resolution, bool isPrimary) :
			Name(name), Offset(offset), Resolution(resolution), IsPrimary(isPrimary)
		{}
	};
}