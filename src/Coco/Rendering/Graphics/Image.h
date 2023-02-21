#pragma once

#include <Coco/Core/Core.h>

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
		int Width;

		/// <summary>
		/// The pixel height of the image
		/// </summary>
		int Height;

		/// <summary>
		/// The pixel depth of the image
		/// </summary>
		int Depth;

		/// <summary>
		/// The number of layers in this image
		/// </summary>
		int Layers;

		/// <summary>
		/// The number of mip maps this image has
		/// </summary>
		int MipCount;

		/// <summary>
		/// The pixel format of this image
		/// </summary>
		PixelFormat PixelFormat;

		/// <summary>
		/// The color space of this image
		/// </summary>
		ColorSpace ColorSpace;

		/// <summary>
		/// The type of dimensions of this image
		/// </summary>
		ImageDimensionType DimensionType;

		ImageDescription();
		ImageDescription(int width, int height, int mipCount, Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace);
		ImageDescription(int width, int height, int depth, int layers, int mipCount, Rendering::PixelFormat pixelFormat, Rendering::ColorSpace colorSpace);
	};

	/// <summary>
	/// A representation of an image on the GPU
	/// </summary>
	class COCOAPI Image : public GraphicsResource
	{
	protected:
		ImageDescription Description;

	protected:
		Image(GraphicsDevice* device, ImageDescription description);

	public:
		virtual ~Image() = default;

		/// <summary>
		/// Gets this image's description
		/// </summary>
		/// <returns>This image's description</returns>
		ImageDescription GetDescription() const { return Description; }
	};
}