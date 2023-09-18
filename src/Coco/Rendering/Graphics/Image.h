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

		virtual uint64 GetSize() const = 0;
		virtual void SetPixels(uint64 offset, const void* pixelData, uint64 size) = 0;
	};
}