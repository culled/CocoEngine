#pragma once

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief A raw image for a GraphicsDevice
	class Image
	{
	public:
		virtual ~Image() = default;

		/// @brief Gets this image's description
		/// @return This image's description
		virtual ImageDescription GetDescription() const = 0;
	};
}