#pragma once

#include "Windowpch.h"
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Size.h>

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

		/// @brief The DPI of the display
		uint16 DPI;

		/// @brief If true, this is the primary display for the system
		bool IsPrimary;

		DisplayInfo();
	};
}