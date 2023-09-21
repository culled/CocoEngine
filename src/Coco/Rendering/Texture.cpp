#include "Renderpch.h"
#include "Texture.h"
#include "Vendor/stb_image.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	Texture::Texture(
		const ResourceID& id,
		const string& name, 
		const ImageDescription& imageDescription, 
		const ImageSamplerDescription& samplerDescription) :
		RendererResource(id, name),
		_version(0),
		_image(),
		_sampler(),
		_imageFilePath()
	{
		RenderService* rendering = EnsureRenderService();

		_image = rendering->GetDevice()->CreateImage(imageDescription);
		_sampler = rendering->GetDevice()->CreateImageSampler(samplerDescription);
	}

	Texture::Texture(
		const ResourceID& id,
		const string& name, 
		const string& imageFilePath, 
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags, 
		const ImageSamplerDescription& samplerDescription) :
		RendererResource(id, name),
		_version(0),
		_image(),
		_sampler(),
		_imageFilePath(imageFilePath)
	{
		RenderService* rendering = EnsureRenderService();

		_sampler = rendering->GetDevice()->CreateImageSampler(samplerDescription);

		ReloadImage(colorSpace, usageFlags);
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

	void Texture::ReloadImage()
	{
		Assert(_image.IsValid())

		ImageDescription desc = _image->GetDescription();
		ReloadImage(desc.ColorSpace, desc.UsageFlags);
	}

	void Texture::ReloadImage(ImageColorSpace colorSpace, ImageUsageFlags usageFlags)
	{
		if (_imageFilePath.empty())
			return;

		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Load in the image data from the file
		int channelsToLoad = 4;
		int actualChannelCount;
		int width, height;
		uint8_t* rawImageData = stbi_load(_imageFilePath.c_str(), &width, &height, &actualChannelCount, channelsToLoad);

		if (rawImageData == nullptr || stbi_failure_reason())
		{
			CocoError("Failed to load image data from \"{}\": {}", _imageFilePath, stbi_failure_reason())

			// Free the image data if it was somehow loaded
			if (rawImageData)
				stbi_image_free(rawImageData);

			// Clears any error so it won't cause subsequent false failures
			stbi_clear_error();

			return;
		}

		
		uint64_t byteSize = static_cast<uint64_t>(width) * height * channelsToLoad;

		// Check if there is any transparency in the image
		bool hasTransparency = false;

		if (actualChannelCount > 3)
		{
			for (uint64_t i = 0; i < byteSize; i += channelsToLoad)
			{
				if (rawImageData[i + 3] < 255)
				{
					hasTransparency = true;
					break;
				}
			}
		}

		ImagePixelFormat pixelFormat;

		// TODO: determine this from loaded file
		switch (channelsToLoad)
		{
		case 4:
			pixelFormat = ImagePixelFormat::RGBA8;
			break;
		default:
		{
			CocoError("Images with {} channels are not supported", channelsToLoad)
			stbi_image_free(rawImageData);
			return;
		}
		}

		usageFlags |= ImageUsageFlags::TransferDestination;
		usageFlags |= ImageUsageFlags::TransferSource;

		// Create a new image description with the given usage flags
		ImageDescription description(static_cast<uint32>(width), static_cast<uint32>(height), 1, pixelFormat, colorSpace, usageFlags);

		// Hold onto the old image data just in-case the transfer fails
		Ref<Image> newImage = _image;

		RenderService* rendering = EnsureRenderService();
		GraphicsDevice* device = rendering->GetDevice();

		try
		{
			// Load the image data into this texture
			newImage = device->CreateImage(description);
			newImage->SetPixels(0, rawImageData, byteSize);
		}
		catch (const std::exception& ex)
		{
			CocoError("Failed to transfer image data into backend: {}", ex.what())

			// Revert to the previous image data
			newImage.Invalidate();
			device->PurgeUnusedResources();

			return;
		}

		_image = newImage;
		device->PurgeUnusedResources();
		_version++;

		stbi_image_free(rawImageData);

		CocoTrace("Loaded image \"{}\"", _imageFilePath)
	}
}