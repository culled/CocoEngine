#include "Renderpch.h"
#include "Texture.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>
#include "Vendor/stb_image.h"

namespace Coco::Rendering
{
	Texture::Texture(const ResourceID& id) :
		Texture(id, ImageDescription::Empty, ImageSamplerDescription::LinearRepeat)
	{}

	Texture::Texture(const ResourceID& id, const ImageDescription& imageDescription, const ImageSamplerDescription& samplerDescription) :
		Resource(id),
		_imageFilePath(),
		_image(),
		_sampler(),
		_imageDescription(imageDescription)
	{
		RenderService* rendering = RenderService::Get();

		CocoAssert(rendering, "RenderService singleton was null")

		AddTextureImageUsageFlags(_imageDescription);

		if(imageDescription != ImageDescription::Empty)
			_image = rendering->GetDevice().CreateImage(_imageDescription);

		CreateSampler(samplerDescription);
	}

	Texture::Texture(
		const ResourceID& id, 
		const FilePath& imageFilePath, 
		ImageColorSpace colorSpace, 
		ImageUsageFlags usageFlags, 
		const ImageSamplerDescription& samplerDescription) :
		Resource(id),
		_imageFilePath(imageFilePath),
		_imageDescription(ImageDescription::Create2D(1, 1, ImagePixelFormat::RGBA8, colorSpace, usageFlags, true)),
		_image(),
		_sampler()
	{
		AddTextureImageUsageFlags(_imageDescription);
		LoadImage(imageFilePath);
		CreateSampler(samplerDescription);
	}

	Texture::~Texture()
	{
		RenderService* rendering = RenderService::Get();
		if (rendering)
		{
			GraphicsDevice& device = rendering->GetDevice();

			if (_sampler.IsValid())
				device.TryReleaseResource(_sampler->ID);

			if (_image.IsValid())
				device.TryReleaseResource(_image->ID);
		}
		else
		{
			_image.Invalidate();
			_sampler.Invalidate();
		}
	}

	void Texture::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		CocoAssert(_image.IsValid(), "Image was invalid")
	
		_image->SetPixels(offset, pixelData, pixelDataSize);
	}

	void Texture::Resize(uint32 width, uint32 height)
	{
		if (width == _imageDescription.Width && height == _imageDescription.Height)
			return;

		CocoAssert(_image.IsValid(), "Image was invalid")

		_imageDescription.Width = width;
		_imageDescription.Height = height;

		if (width == 1 || height == 1)
			_imageDescription.DimensionType = ImageDimensionType::OneDimensional;
		else
			_imageDescription.DimensionType = ImageDimensionType::TwoDimensional;

		RenderService* rendering = RenderService::Get();
		GraphicsDevice& device = rendering->GetDevice();

		Ref<Image> oldImage = _image;
		_image = device.CreateImage(_imageDescription);

		device.TryReleaseResource(oldImage->ID);
	}
	
	//void Texture::ReadPixel(const Vector2Int& pixelCoordinate, void* outData, size_t dataSize)
	//{
	//	CocoAssert(_image.IsValid(), "Image was invalid")
	//
	//	_image->ReadPixel(pixelCoordinate, outData, dataSize);
	//}

	void Texture::ReloadImage()
	{
		LoadImage(_imageFilePath);
	}

	void Texture::AddTextureImageUsageFlags(ImageDescription& description)
	{
		description.UsageFlags |= ImageUsageFlags::Sampled;
		description.UsageFlags |= ImageUsageFlags::TransferDestination;
		description.UsageFlags |= ImageUsageFlags::TransferSource;
	}

	void Texture::LoadImage(const FilePath& imageFilePath)
	{
		if (imageFilePath.IsEmpty())
			return;

		if(_imageDescription.PixelFormat != ImagePixelFormat::RGBA8)
		{ 
			CocoError("Loading of RGBA8 is currently the only supported image format")
			return;
		}

		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Load in the image data from the file
		int channelsToLoad = GetPixelFormatChannelCount(_imageDescription.PixelFormat);
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
			{
				stbi_image_free(rawImageData);
				rawImageData = nullptr;
			}

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

		// Create a new image description with the given usage flags
		ImageDescription description = ImageDescription::Create2D(
			static_cast<uint32>(width), static_cast<uint32>(height), 
			_imageDescription.PixelFormat, _imageDescription.ColorSpace, 
			_imageDescription.UsageFlags,
			true);

		// Hold onto the old image data just in-case the transfer fails
		Ref<Image> newImage = _image;

		GraphicsDevice& device = rendering->GetDevice();

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
			device.TryReleaseResource(newImage->ID);

			return;
		}

		stbi_image_free(rawImageData);

		Ref<Image> oldImage = _image;
		_image = newImage;

		if (oldImage)
			device.TryReleaseResource(oldImage->ID);

		_imageFilePath = imageFilePath;
		_imageDescription = description;
		IncrementVersion();

		CocoTrace("Loaded image \"{}\"", _imageFilePath.ToString())
	}

	void Texture::CreateSampler(const ImageSamplerDescription& samplerDescription)
	{
		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

		GraphicsDevice& device = rendering->GetDevice();

		if (_sampler)
			device.TryReleaseResource(_sampler->ID);

		ImageSamplerDescription newSamplerDescription = samplerDescription;
		if (samplerDescription.MaxLOD == 0)
			newSamplerDescription.MaxLOD = _imageDescription.MipCount;

		_sampler = device.CreateImageSampler(newSamplerDescription);
	}
}