#include "Texture.h"

#include "Graphics/GraphicsPlatform.h"
#include "RenderingService.h"
#include <Coco/Core/Engine.h>

#include <Coco/Vendor/stb/stb_image.h>

namespace Coco::Rendering
{
	Texture::Texture(const ResourceID& id, const string& name) : RenderingResource(id, name), 
		_usageFlags(ImageUsageFlags::None)
	{}

	Texture::Texture(
		const ResourceID& id,
		const string& name,
		int width,
		int height,
		PixelFormat pixelFormat,
		ColorSpace colorSpace,
		ImageUsageFlags usageFlags,
		const ImageSamplerProperties& samplerProperties
	) : RenderingResource(id, name),
		_usageFlags(usageFlags), 
		_samplerProperties(samplerProperties)
	{
		RecreateImageFromDescription(ImageDescription(width, height, 1, pixelFormat, colorSpace, usageFlags));
		RecreateInternalSampler();
	}

	Texture::Texture(
		const ResourceID& id,
		const string& name,
		const ImageDescription& description, 
		const ImageSamplerProperties& samplerProperties
	) : RenderingResource(id, name),
		_usageFlags(description.UsageFlags), 
		_samplerProperties(samplerProperties)
	{
		RecreateImageFromDescription(description);
		RecreateInternalSampler();
	}

	Texture::Texture(
		const ResourceID& id,
		const string& name,
		const string& filePath, 
		ImageUsageFlags usageFlags, 
		const ImageSamplerProperties& samplerProperties,
		int channelCount
	) : RenderingResource(id, name),
		_usageFlags(usageFlags), 
		_samplerProperties(samplerProperties)
	{
		LoadFromFile(filePath, channelCount);
		RecreateInternalSampler();
	}

	Texture::~Texture()
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		if(_image.IsValid())
			platform->PurgeResource(_image);

		if(_sampler.IsValid())
			platform->PurgeResource(_sampler);
	}

	ImageDescription Texture::GetDescription() const noexcept
	{
		if (_image.IsValid())
			return _image->GetDescription();
		
		return ImageDescription::Empty;
	}

	void Texture::SetPixels(uint64_t offset, uint64_t size, const void* pixelData)
	{
		_image->SetPixelData(offset, size, pixelData);
		IncrementVersion();
	}

	void Texture::SetSamplerProperties(const ImageSamplerProperties& samplerProperties)
	{
		_samplerProperties = samplerProperties;

		RecreateInternalSampler();
		IncrementVersion();
	}

	bool Texture::LoadFromFile(const string& filePath, int imageChannelCount)
	{
		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Create a new texture description with the given usage flags
		ImageDescription description = {};
		description.UsageFlags = _usageFlags;

		// Load in the image data from the file
		int actualChannelCount;
		uint8_t* rawImageData = stbi_load(filePath.c_str(), &description.Width, &description.Height, &actualChannelCount, imageChannelCount);

		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		if (rawImageData == nullptr || stbi_failure_reason())
		{
			LogError(platform->GetLogger(), FormattedString("Failed to load image data from \"{}\": {}", filePath, stbi_failure_reason()));

			// Free the image data if it was somehow loaded
			if (rawImageData != nullptr)
				stbi_image_free(rawImageData);

			// Clears any error so it won't cause subsequent false failures
			stbi_clear_error();

			return false;
		}

		uint64_t byteSize = static_cast<uint64_t>(description.Width) * description.Height * imageChannelCount;

		// Check if there is any transparency in the image
		bool hasTransparency = false;

		if (imageChannelCount > 3)
		{
			for (uint64_t i = 0; i < byteSize; i += imageChannelCount)
			{
				if (rawImageData[i + 3] < 255)
				{
					hasTransparency = true;
					break;
				}
			}
		}

		// TODO: determine this from loaded file
		description.PixelFormat = PixelFormat::RGBA8;
		description.ColorSpace = ColorSpace::sRGB;
		description.DimensionType = description.Height == 1 ? ImageDimensionType::OneDimensional : ImageDimensionType::TwoDimensional;

		// Hold onto the old image data just in-case the transfer fails
		ResourceVersion oldVersion = GetVersion();
		Ref<Image> oldImage = _image;

		try
		{
			// Load the image data into this texture
			_image = platform->CreateImage(_name, description);
			_image->SetPixelData(0, byteSize, rawImageData);
		}
		catch (const Exception& ex)
		{
			LogError(platform->GetLogger(), FormattedString("Failed to transfer image data into backend: {}", ex.what()));

			// Revert to the previous image data
			platform->PurgeResource(_image);
			_image = oldImage;
			SetVersion(oldVersion);

			return false;
		}

		if(oldImage.IsValid())
			platform->PurgeResource(oldImage);

		IncrementVersion();

		_imageFilePath = filePath;

		stbi_image_free(rawImageData);

		LogTrace(platform->GetLogger(), FormattedString("Loaded image \"{}\"", filePath));

		return true;
	}

	void Texture::RecreateImageFromDescription(const ImageDescription& newDescription)
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		if(_image.IsValid())
			platform->PurgeResource(_image);

		_image = platform->CreateImage(_name, newDescription);
	}

	void Texture::RecreateInternalSampler()
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		if (_sampler.IsValid())
			platform->PurgeResource(_sampler);

		_sampler = platform->CreateImageSampler(_name, _samplerProperties);
	}
}
