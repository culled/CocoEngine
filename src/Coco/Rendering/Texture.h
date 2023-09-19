#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief Holds an image and an image sampler
	class Texture
	{
	private:
		Ref<Image> _image;
		Ref<ImageSampler> _sampler;

	public:
		Texture(
			const ImageDescription& imageDescription,
			const ImageSamplerDescription& samplerDescription
		);

		~Texture();

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
	};
}