#pragma once

#include <Coco/Core/Types/Vector.h>

namespace Coco
{
	struct UIUtils
	{
		static bool DrawVector3UI(uint64 id, const char* label, Vector3& vector, double resetValue = 0.0);
	};
}