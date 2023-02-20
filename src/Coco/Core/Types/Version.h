#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/String.h>

namespace Coco
{
	struct COCOAPI Version
	{
		int Major;
		int Minor;
		int Patch;

		Version();
		Version(int major, int minor, int patch);

		string ToString() const;
	};
}