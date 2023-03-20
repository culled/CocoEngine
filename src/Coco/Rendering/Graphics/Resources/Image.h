#pragma once

#include "GraphicsResource.h"

#include "ImageTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A description of an Image
	/// </summary>
	struct COCOAPI ImageDescription
	{
		/// <summary>
		/// An empty image description
		/// </summary>
		static const ImageDescription Empty;

		/// <summary>
		/// The pixel width of the image
		/// </summary>
		int Width = 1;

		/// <summary>
		/// The pixel height of the image
		/// </summary>
		int Height = 1;

		/// <summary>
		/// The pixel depth of the image
		/// </summary>
		int Depth = 1;

		/// <summary>
		/// The number of layers in this image
		/// </summary>
		int Layers = 1;

		/// <summary>
		/// The number of mip maps this image has
		/// </summary>
		int MipCount = 1;

		/// <summary>
		/// The pixel format of this image
		/// </summary>
		PixelFormat PixelFormat = PixelFormat::Unknown;

		/// <summary>
		/// The color space of this image
		/// </summary>
		ColorSpace ColorSpace = ColorSpace::Unknown;

		/// <summary>
		/// The type of dimensions of this image
		/// </summary>
		ImageDimensionType DimensionType = ImageDimensionType::TwoDimensional;

		/// <summary>
		/// Usage flags for this image
		/// </summary>
		ImageUsageFlags UsageFlags = ImageUsageFlags::None;

		ImageDescription() = default;
		ImageDescription(int width, int height, 
			int mipCount, 
			Rendering::PixelFormat pixelFormat, 
			Rendering::ColorSpace colorSpace, 
			ImageUsageFlags usageFlags) noexcept :
			ImageDescription(width, height, 1, 1, mipCount, pixelFormat, colorSpace, usageFlags) {}
		ImageDescription(
			int width, int height, int depth, 
			int layers, int mipCount, 
			Rendering::PixelFormat pixelFormat, 
			Rendering::ColorSpace colorSpace, 
			ImageUsageFlags usageFlags) noexcept;
	};

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