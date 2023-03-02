#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	class COCOAPI Random
	{
	public:
		static int RandomRange(int min, int max);
		static double RandomRange(double min, double max);

		static double RandomValue();
	};
}