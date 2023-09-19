#pragma once

#include "../Renderpch.h"
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

		/// @brief Gets the size of this image's data, in bytes
		/// @return The size of this image's data
		virtual uint64 GetSize() const = 0;

		/// @brief Sets the pixels of this image
		/// @param offset The offset in the image buffer
		/// @param pixelData The pixel data
		/// @param pixelDataSize The size of the pixel data
		virtual void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize) = 0;
	};
}