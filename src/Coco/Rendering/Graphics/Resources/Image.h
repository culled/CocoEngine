#pragma once

#include "GraphicsResource.h"

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// @brief A representation of an image on the GPU
	class COCOAPI Image : public IGraphicsResource
	{
	protected:
		/// @brief The description for this image
		ImageDescription Description;

	protected:
		Image(ImageDescription description) noexcept;

	public:
		virtual ~Image() = default;

		Image(const Image&) = delete;
		Image(Image&&) = delete;

		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) = delete;

		/// @brief Gets this image's description
		/// @return This image's description
		const ImageDescription& GetDescription() const noexcept { return Description; }

		/// @brief Gets the size (in bytes) of the memory that this image requires
		/// @return The image size (in bytes)
		uint64_t GetSize() const noexcept
		{
			return static_cast<uint64_t>(Description.Width) * Description.Height * Description.Depth * GetPixelFormatSize(Description.PixelFormat);
		}

		/// @brief Sets pixel data to this image. NOTE: make sure the type matches the pixel format of this image!
		/// @param offset The offset in the image memory to start loading pixel data into
		/// @param size The size of the pixel data being loaded
		/// @param pixelData The pixel data
		virtual void SetPixelData(uint64_t offset, uint64_t size, const void* pixelData) = 0;
	};
}