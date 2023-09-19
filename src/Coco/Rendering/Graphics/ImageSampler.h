#pragma once

#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	/// @brief A sampler for an Image
	class ImageSampler
	{
	public:
		virtual ~ImageSampler() = default;

		/// @brief Gets this image sampler's description
		/// @return The description
		virtual ImageSamplerDescription GetDescription() const = 0;
	};
}