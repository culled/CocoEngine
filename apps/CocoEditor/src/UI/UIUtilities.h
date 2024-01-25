#pragma once

#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco
{
	struct UIUtilities
	{
		static bool DrawInputStringEdit(uint64 id, const char* label, string& str, int maxLength);
		static bool DrawVector3UI(uint64 id, const char* label, Vector3& vector, double resetValue = 0.0);
	};
}