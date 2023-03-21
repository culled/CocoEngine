#pragma once

#include "GraphicsResource.h"

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A representation of an image on the GPU
	/// </summary>
	class COCOAPI Image : public IGraphicsResource
	{
	protected:
		ImageDescription Description;

	protected:
		Image(ImageDescription description) noexcept;

	public:
		virtual ~Image() = default;

		Image(const Image&) = delete;
		Image(Image&&) = delete;

		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) = delete;

		/// <summary>
		/// Gets this image's description
		/// </summary>
		/// <returns>This image's description</returns>
		ImageDescription GetDescription() const noexcept { return Description; }

		/// <summary>
		/// Gets the size (in bytes) of the memory that this image requires
		/// </summary>
		/// <returns>The image size (in bytes)</returns>
		uint64_t GetSize() const noexcept
		{
			return static_cast<uint64_t>(Description.Width) * Description.Height * Description.Depth * GetPixelFormatSize(Description.PixelFormat);
		}

		/// <summary>
		/// Sets pixel data to this image.
		/// NOTE: make sure the type matches the pixel format of this image!
		/// </summary>
		/// <param name="offset">The offset in the image memory to start loading pixel data into</param>
		/// <param name="size">The size of the pixel data being loaded</param>
		/// <param name="pixelData">The pixel data</param>
		virtual void SetPixelData(uint64_t offset, uint64_t size, const void* pixelData) = 0;
	};
}