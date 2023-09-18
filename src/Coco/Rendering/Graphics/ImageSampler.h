#pragma once

#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	class ImageSampler
	{
	public:
		virtual ~ImageSampler() = default;

		virtual ImageSamplerDescription GetDescription() const = 0;
	};
}