#include "Texture.h"

#include "Graphics/GraphicsPlatform.h"
#include "RenderingService.h"
#include <Coco/Core/Engine.h>

#include <Coco/Vendor/stb/stb_image.h>

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
		uint maxAnisotropy
	) : RenderingResource(ResourceType::Texture),
		_usageFlags(usageFlags), _filterMode(filterMode), _repeatMode(repeatMode), _maxAnisotropy(maxAnisotropy)
	{
		RecreateImageFromDescription(ImageDescription(width, height, 1, pixelFormat, colorSpace, usageFlags));
		RecreateInternalSampler();
	}

	Texture::Texture(
		const ImageDescription& description, 
		FilterMode filterMode, 
		RepeatMode repeatMode, 
		uint maxAnisotropy
	) : RenderingResource(ResourceType::Texture),
		_usageFlags(description.UsageFlags), _filterMode(filterMode), _repeatMode(repeatMode), _maxAnisotropy(maxAnisotropy)
	{
		RecreateImageFromDescription(description);
		RecreateInternalSampler();
	}

	Texture::Texture(
		const string& filePath, 
		ImageUsageFlags usageFlags, 
		FilterMode filterMode, 
		RepeatMode repeatMode, 
		uint maxAnisotropy, 
		int channelCount
	) : RenderingResource(ResourceType::Texture),
		_usageFlags(usageFlags), _filterMode(filterMode), _repeatMode(repeatMode), _maxAnisotropy(maxAnisotropy)
	{
		LoadFromFile(filePath, channelCount);
		RecreateInternalSampler();
	}

	Texture::~Texture()
	{
		_sampler.Invalidate();
		_image.Invalidate();
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

	void Texture::SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint maxAnisotropy)
	{
		_repeatMode = repeatMode;
		_filterMode = filterMode;
		_maxAnisotropy = maxAnisotropy;

		RecreateInternalSampler();
		IncrementVersion();
	}

	bool Texture::LoadFromFile(const string& filePath, int imageChannelCount)
	{
		RenderingService* renderService = EnsureRenderingService();

		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Create a new texture description with the given usage flags
		ImageDescription description = {};
		description.UsageFlags = _usageFlags;

		const string fullFilePath = Engine::Get()->GetResourceLibrary()->BasePath + filePath;

		// Load in the image data from the file
		int actualChannelCount;
		uint8_t* rawImageData = stbi_load(fullFilePath.c_str(), &description.Width, &description.Height, &actualChannelCount, imageChannelCount);

		if (rawImageData == nullptr || stbi_failure_reason())
		{
			LogError(renderService->GetLogger(), FormattedString("Failed to load image data from \"{}\": {}", filePath, stbi_failure_reason()));

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
		WeakManagedRef<Image> oldImage = _image;

		try
		{
			// Load the image data into this texture
			RecreateImageFromDescription(description);
			SetPixels(0, byteSize, rawImageData);
			_imageFilePath = filePath;
		}
		catch (const Exception& ex)
		{
			LogError(renderService->GetLogger(), FormattedString("Failed to transfer image data into backend: {}", ex.what()));

			// Revert to the previous image data
			_image = oldImage;
			SetVersion(oldVersion);

			return false;
		}

		stbi_image_free(rawImageData);

		LogTrace(renderService->GetLogger(), FormattedString("Loaded image \"{}\"", filePath));

		return true;
	}

	void Texture::RecreateImageFromDescription(const ImageDescription& newDescription)
	{
		_image = EnsureRenderingService()->GetPlatform()->CreateImage(newDescription);
	}

	void Texture::RecreateInternalSampler()
	{
		_sampler = EnsureRenderingService()->GetPlatform()->CreateImageSampler(_filterMode, _repeatMode, _maxAnisotropy);
	}
}
