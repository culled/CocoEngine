#include "Windowpch.h"
#include "DisplayInfo.h"

namespace Coco::Windowing
{
	DisplayInfo::DisplayInfo() :
		Name{},
		Offset(Vector2Int::Zero),
		Resolution(SizeInt::Zero),
		DPI(0),
		IsPrimary(false)
	{}
}