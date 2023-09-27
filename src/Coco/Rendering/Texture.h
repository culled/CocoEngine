#pragma once

#include "RendererResource.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief Holds an image and an image sampler
	class Texture : public RendererResource
	{
	private:
		uint64 _version;
		Ref<Image> _image;
		Ref<ImageSampler> _sampler;
		string _imageFilePath;

	public:
		Texture(
			const ResourceID& id,
			const string& name,
			const ImageDescription& imageDescription,
			const ImageSamplerDescription& samplerDescription
		);

		Texture(
			const ResourceID& id,
			const string& name,
			const string& imageFilePath,
			ImageColorSpace colorSpace,
			ImageUsageFlags usageFlags,
			const ImageSamplerDescription& samplerDescription
		);

		~Texture();

		std::type_index GetType() const final { return typeid(Texture); }

		/// @brief Gets the image
		/// @return The image
		Ref<Image> GetImage() const { return _image; }

		/// @brief Gets the image sampler
		/// @return The image sampler
		Ref<ImageSampler> GetImageSampler() const { return _sampler; }

		/// @brief Sets the pixel data for this texture
		/// @param offset The offset in the texture memory to start loading pixel data into
		/// @param pixelData The pixel data
		/// @param pixelDataSize The size of the pixel data
		void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize);

		/// @brief Reload this texture's image from the image file
		void ReloadImage();

	private:
		/// @brief Reloads this texture's image from the image file
		/// @param colorSpace The color space to load the image as
		/// @param usageFlags The usage flags for the image
		void ReloadImage(ImageColorSpace colorSpace, ImageUsageFlags usageFlags);

		/// @brief Creates this texture's sampler
		/// @param samplerDescription The description for the sampler
		void CreateSampler(const ImageSamplerDescription& samplerDescription);
	};
}