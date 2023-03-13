#pragma once

#include <Coco/Core/Core.h>

#include "ImageTypes.h"
#include "ImageSampler.h"
#include "GraphicsResource.h"

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
			ImageUsageFlags usageFlags);
		ImageDescription(
			int width, int height, int depth, 
			int layers, int mipCount, 
			Rendering::PixelFormat pixelFormat, 
			Rendering::ColorSpace colorSpace, 
			ImageUsageFlags usageFlags);
	};

	/// <summary>
	/// A representation of an image on the GPU
	/// </summary>
	class COCOAPI Image : public IGraphicsResource
	{
	protected:
		ImageDescription Description;

	protected:
		Image(ImageDescription description);

	public:
		virtual ~Image() = default;

		/// <summary>
		/// Gets this image's description
		/// </summary>
		/// <returns>This image's description</returns>
		ImageDescription GetDescription() const { return Description; }

		/// <summary>
		/// Sets pixel data to this image.
		/// NOTE: make sure the type matches the pixel format of this image!
		/// </summary>
		/// <param name="pixelData">Raw pixel data</param>
		virtual void SetPixelData(const void* pixelData) = 0;
	};
}