#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
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

		Ref<Image> GetImage() const { return _image; }
		Ref<ImageSampler> GetImageSampler() const { return _sampler; }

		/// @brief Sets the pixel data for this texture
		/// @param offset The offset in the texture memory to start loading pixel data into
		/// @param pixelData The pixel data
		/// @param size The size of the pixel data being loaded 
		void SetPixels(uint64 offset, const void* pixelData, uint64 size);
	};
}