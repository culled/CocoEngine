#include "Texture.h"

namespace Coco::Rendering
{
	Texture::Texture(int width, int height, PixelFormat pixelFormat, ColorSpace colorSpace, ImageUsageFlags usageFlags, GraphicsPlatform* platform) :
		_platform(platform), _refreshCount(0)
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

	void Texture::SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint anisotropy)
	{
		_repeatMode = repeatMode;
		_filterMode = filterMode;
		_anisotropy = anisotropy;

		RecreateInternalSampler();
	}

	void Texture::RecreateInternalImage()
	{
		_image = _platform->CreateImage(_description);
		_refreshCount++;
	}

	void Texture::RecreateInternalSampler()
	{
		_sampler = _platform->CreateImageSampler(_filterMode, _repeatMode, _anisotropy);
		_refreshCount++;
	}
}
