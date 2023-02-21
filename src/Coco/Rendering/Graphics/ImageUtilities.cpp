#include "ImageUtilities.h"

namespace Coco::Rendering
{
	bool IsDepthStencilFormat(PixelFormat format)
	{
		return format == PixelFormat::Depth24_Stencil8;
	}
}