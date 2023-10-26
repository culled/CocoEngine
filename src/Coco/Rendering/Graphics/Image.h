#pragma once

#include "../Renderpch.h"
#include "ImageTypes.h"
#include <Coco/Core/Types/Vector.h>

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
		virtual uint64 GetDataSize() const = 0;

		/// @brief Sets the pixels of this image
		/// @param offset The offset in the image buffer
		/// @param pixelData The pixel data
		/// @param pixelDataSize The size of the pixel data
		virtual void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize) = 0;

		/// @brief Sets the pixels of this image
		/// @tparam DataType The type of pixel data
		/// @param offset The offset in the image buffer 
		/// @param data The pixel data
		template<typename DataType>
		void SetPixels(uint64 offset, std::span<const DataType> data)
		{
			SetPixels(offset, data.data(), data.size() * sizeof(DataType));
		}

		/// @brief Gets the value of a pixel at the given position
		/// @param pixelCoords The coordinates of the pixel
		/// @param outData The destination for the pixel data copy. This should match the expected pixel data type
		/// @param dataSize The size of the outData structure
		virtual void ReadPixel(const Vector2Int& pixelCoords, void* outData, size_t dataSize) = 0;

		/// @brief Gets the value of a pixel at the given position 
		/// @tparam DataType The type of pixel data
		/// @param pixelCoords The coordinates of the pixel
		/// @return The pixel value
		template<typename DataType>
		DataType ReadPixel(const Vector2Int& pixelCoords)
		{
			DataType r{};
			ReadPixel(pixelCoords, &r, sizeof(DataType));
			return r;
		}
	};
}