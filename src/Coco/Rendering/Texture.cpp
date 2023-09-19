#include "Renderpch.h"
#include "Texture.h"

#include "RenderService.h"
namespace Coco::Rendering
{
	Texture::Texture(const ImageDescription& imageDescription, const ImageSamplerDescription& samplerDescription) :
		_image(),
		_sampler()
	{
		RenderService* rendering = RenderService::Get();
		if (!rendering)
			throw std::exception("No active RenderService was found");

		_image = rendering->GetDevice()->CreateImage(imageDescription);
		_sampler = rendering->GetDevice()->CreateImageSampler(samplerDescription);
	}

	Texture::~Texture()
	{
		if (_image.IsValid())
			_image.Invalidate();

		if (_sampler.IsValid())
			_sampler.Invalidate();

		RenderService* rendering = RenderService::Get();
		if (rendering)
			rendering->GetDevice()->PurgeUnusedResources();
	}

	void Texture::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		Assert(_image.IsValid())

		_image->SetPixels(offset, pixelData, pixelDataSize);
	}
}