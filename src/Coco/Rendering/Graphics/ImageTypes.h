#pragma once

#include <Coco/Core/Defines.h>
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief Image pixel formats
	enum class ImagePixelFormat
	{
		/// @brief R, G, B, and A channels, each storing 8 bits
		RGBA8,

		/// @brief R channel storing a plain, 32-bit integer
		R32_Int,

		/// @brief 32 bit depth channel with an 8 bit stencil channel
		Depth32_Stencil8,

		/// @brief An unknown format
		Unknown
	};

	/// @brief Image color spaces
	enum class ImageColorSpace
	{
		/// @brief sRGB color space
		sRGB,

		/// @brief Linear color space
		Linear,

		/// @brief Unknown color space
		Unknown
	};

	/// @brief Image dimension types
	enum class ImageDimensionType
	{
		/// @brief An image with only a width
		OneDimensional,

		/// @brief An array of 1D images
		OneDimensionalArray,

		/// @brief An image with a width and height
		TwoDimensional,

		/// @brief An array of 2D images
		TwoDimensionalArray,

		/// @brief An image with a width, height, and depth
		ThreeDimensional,

		/// @brief An array of 3D images
		ThreeDimensionalArray,

		/// @brief An image that can be used as a cubemap
		CubeMap,

		/// @brief An array of cubemap images
		CubeMapArray
	};

	/// @brief Usage flags for an image
	enum class ImageUsageFlags
	{
		None = 0,

		/// @brief Allows the image to be the source of a transfer operation
		TransferSource = 1 << 0,

		/// @brief Allows the image to be the destination of a transfer operation
		TransferDestination = 1 << 1,

		/// @brief Allows the image to be sampled from shaders
		Sampled = 1 << 2,

		/// @brief Allows the image to be written to by shaders
		RenderTarget = 1 << 3,

		/// @brief Allows the image to be presented by a presenter
		Presented = 1 << 4,

		/// @brief Allows image data to be access via the CPU
		HostVisible = 1 << 5,
	};

	DefineFlagOperators(ImageUsageFlags)

	/// @brief A description of an Image
	struct ImageDescription
	{
		/// @brief An empty image description
		static const ImageDescription Empty;

		/// @brief The pixel width of the image
		uint32 Width;

		/// @brief The pixel height of the image
		uint32 Height;

		/// @brief The pixel depth of the image
		uint32 Depth;

		/// @brief The number of layers in this image
		uint32 Layers;

		/// @brief The number of mip maps this image has
		uint32 MipCount;

		/// @brief The pixel format of this image
		ImagePixelFormat PixelFormat;

		/// @brief The color space of this image
		ImageColorSpace ColorSpace;

		/// @brief The type of dimensions of this image
		ImageDimensionType DimensionType;

		/// @brief Usage flags for this image
		ImageUsageFlags UsageFlags;

		/// @brief The number of MSAA samples this image supports
		MSAASamples SampleCount;

		ImageDescription();

		ImageDescription(
			uint32 width, uint32 height, uint32 depth,
			uint32 layers,
			ImagePixelFormat pixelFormat,
			ImageColorSpace colorSpace,
			ImageUsageFlags usageFlags,
			bool withMipMaps,
			MSAASamples sampleCount = MSAASamples::One);

		bool operator==(const ImageDescription& other) const;

		static ImageDescription Create2D(
			uint32 width, uint32 height,
			ImagePixelFormat pixelFormat,
			ImageColorSpace colorSpace,
			ImageUsageFlags usageFlags,
			bool withMipMaps,
			MSAASamples sampleCount = MSAASamples::One);
	};

	/// @brief Calculates the number of mip maps for an image with the given dimensions
	/// @param width The image width
	/// @param height The image height
	/// @return The number of mip map levels, including the original image
	uint32 CalculateMipMapCount(uint32 width, uint32 height);

	/// @brief Gets the number of bytes per pixel for a pixel format
	/// @param format The pixel format
	/// @return The number of bytes per pixel
	uint8 GetPixelFormatBytesPerPixel(ImagePixelFormat format);

	/// @brief Gets the number of channels for a pixel format
	/// @param format The pixel format
	/// @return The number of channels
	uint8 GetPixelFormatChannelCount(ImagePixelFormat format);

	/// @brief Determines if the given format is a depth format
	/// @param format The format
	/// @return True if the format is a depth format
	bool IsDepthFormat(ImagePixelFormat format);

	/// @brief Determines if the given format is a stencil format
	/// @param format The format
	/// @return True if the format is a stencil format
	bool IsStencilFormat(ImagePixelFormat format);
}