#include "Corepch.h"
#include "Size.h"

namespace Coco
{
	const SizeInt SizeInt::Zero = SizeInt(0, 0);

	SizeInt::SizeInt(uint32 width, uint32 height) : SizeBase<uint32>(width, height)
	{}
}