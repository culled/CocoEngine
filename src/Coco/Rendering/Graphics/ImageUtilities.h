#pragma once

#include <Coco/Core/Core.h>

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Gets if the given pixel format is a depth/stencil format
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>True if the format is a depth/stencil format</returns>
	bool COCOAPI IsDepthStencilFormat(PixelFormat format);
}