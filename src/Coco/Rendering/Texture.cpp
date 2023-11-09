#include "Renderpch.h"
#include "Texture.h"
#include "Vendor/stb_image.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	Texture::Texture(const ResourceID& id, const string& name) :
		RendererResource(id, name),
		_version(0),
		_image(),
		_sampler(),
		_imageFilePath()
	{}

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
		RenderService& rendering = EnsureRenderService();

		_image = rendering.GetDevice().CreateImage(imageDescription);
		CreateSampler(samplerDescription);
	}

	Texture::Texture(
		const ResourceID& id,
		const string& name, 
		const FilePath& imageFilePath,
		ImageColorSpace colorSpace,
		ImageUsageFlags usageFlags, 
		const ImageSamplerDescription& samplerDescription) :
		RendererResource(id, name),
		_version(0),
		_image(),
		_sampler(),
		_imageFilePath(imageFilePath)
	{
		RenderService& rendering = EnsureRenderService();

		LoadImage(imageFilePath, colorSpace, usageFlags);
		CreateSampler(samplerDescription);
	}

	Texture::~Texture()
	{
		RenderService* rendering = RenderService::Get();
		if (rendering)
		{
			GraphicsDevice& device = rendering->GetDevice();
			device.TryReleaseImageSampler(_sampler);
			device.TryReleaseImage(_image);
		}
		else
		{
			_image.Invalidate();
			_sampler.Invalidate();
		}
	}

	void Texture::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		Assert(_image.IsValid())

		_image->SetPixels(offset, pixelData, pixelDataSize);
	}

	void Texture::ReadPixel(const Vector2Int& pixelCoordinate, void* outData, size_t dataSize)
	{
		_image->ReadPixel(pixelCoordinate, outData, dataSize);
	}

	void Texture::ReloadImage()
	{
		Assert(_image.IsValid())

		ImageDescription desc = _image->GetDescription();
		LoadImage(_imageFilePath, desc.ColorSpace, desc.UsageFlags);
	}

	void Texture::LoadImage(const FilePath& imageFilePath, ImageColorSpace colorSpace, ImageUsageFlags usageFlags)
	{
		if (imageFilePath.IsEmpty())
			return;

		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Load in the image data from the file
		int channelsToLoad = 4;
		int actualChannelCount;
		int width, height;

		File f = Engine::Get()->GetFileSystem().OpenFile(imageFilePath, FileOpenFlags::Read);
		std::vector<uint8> fileData = f.ReadToEnd();
		f.Close();

		uint8_t* rawImageData = stbi_load_from_memory(fileData.data(), static_cast<int>(fileData.size()), &width, &height, &actualChannelCount, channelsToLoad);

		if (rawImageData == nullptr || stbi_failure_reason())
		{
			CocoError("Failed to load image data from \"{}\": {}", imageFilePath.ToString(), stbi_failure_reason())

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
		ImageDescription description(static_cast<uint32>(width), static_cast<uint32>(height), pixelFormat, colorSpace, usageFlags, true);

		// Hold onto the old image data just in-case the transfer fails
		Ref<Image> newImage = _image;

		RenderService& rendering = EnsureRenderService();
		GraphicsDevice& device = rendering.GetDevice();

		try
		{
			// Load the image data into this texture
			newImage = device.CreateImage(description);
			newImage->SetPixels(0, rawImageData, byteSize);
		}
		catch (const std::exception& ex)
		{
			CocoError("Failed to transfer image data into backend: {}", ex.what())

			// Revert to the previous image data
			device.TryReleaseImage(newImage);

			return;
		}

		Ref<Image> oldImage = _image;
		_image = newImage;
		device.TryReleaseImage(oldImage);

		_imageFilePath = imageFilePath;
		_version++;

		stbi_image_free(rawImageData);

		CocoTrace("Loaded image \"{}\"", _imageFilePath.ToString())
	}

	void Texture::CreateSampler(const ImageSamplerDescription& samplerDescription)
	{
		RenderService& rendering = EnsureRenderService();

		if (_sampler)
		{
			rendering.GetDevice().TryReleaseImageSampler(_sampler);
		}

		ImageSamplerDescription newSamplerDescription = samplerDescription;
		if(samplerDescription.MaxLOD == 0)
			newSamplerDescription.MaxLOD = _image->GetDescription().MipCount;

		_sampler = rendering.GetDevice().CreateImageSampler(newSamplerDescription);
	}
}