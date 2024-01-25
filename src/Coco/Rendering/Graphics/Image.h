#pragma once

#include "GraphicsResource.h"
#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief An image that holds pixel data
	class Image :
		public GraphicsResource
	{
	public:
		virtual ~Image() = default;

		/// @brief Gets this image's description
		/// @return The ImageDescription
		virtual const ImageDescription& GetDescription() const = 0;

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

	protected:
		Image(const GraphicsResourceID& id);
	};
}