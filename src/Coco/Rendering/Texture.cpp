#include "Texture.h"

#include "Graphics/GraphicsPlatform.h"

namespace Coco::Rendering
{
	Texture::Texture(
		int width,
		int height,
		PixelFormat pixelFormat,
		ColorSpace colorSpace,
		ImageUsageFlags usageFlags,
		FilterMode filterMode,
		RepeatMode repeatMode,
		uint maxAnisotropy,
		GraphicsPlatform* platform) :
		_platform(platform), _refreshCount(0), _filterMode(filterMode), _repeatMode(repeatMode), _maxAnisotropy(maxAnisotropy)
	{
		_description.Width = width;
		_description.Height = height;
		_description.PixelFormat = pixelFormat;
		_description.ColorSpace = colorSpace;
		_description.UsageFlags = usageFlags;

		RecreateInternalImage();
		RecreateInternalSampler();
	}

	Texture::~Texture()
	{
		_image.reset();
	}

	void Texture::SetPixels(const void* pixelData)
	{
		_image->SetPixelData(pixelData);
	}

	void Texture::SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint maxAnisotropy)
	{
		_repeatMode = repeatMode;
		_filterMode = filterMode;
		_maxAnisotropy = maxAnisotropy;

		RecreateInternalSampler();
	}

	void Texture::RecreateInternalImage()
	{
		_image = _platform->CreateImage(_description);
		_refreshCount++;
	}

	void Texture::RecreateInternalSampler()
	{
		_sampler = _platform->CreateImageSampler(_filterMode, _repeatMode, _maxAnisotropy);
		_refreshCount++;
	}
}
