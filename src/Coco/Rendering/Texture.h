#pragma once

#include "RenderingResource.h"

#include "Graphics/Resources/Image.h"
#include "Graphics/Resources/ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief A texture that can be used for rendering images
	class COCOAPI Texture : public RenderingResource
	{
		friend class TextureSerializer;

	private:
		Ref<Image> _image;
		ImageUsageFlags _usageFlags;
		Ref<ImageSampler> _sampler;
		ImageSamplerProperties _samplerProperties;
		string _imageFilePath;

	public:
		Texture(
			ResourceID id,
			const string& name);

		Texture(
			ResourceID id,
			const string& name,
			int width, 
			int height, 
			PixelFormat pixelFormat, 
			ColorSpace colorSpace, 
			ImageUsageFlags usageFlags, 
			const ImageSamplerProperties& samplerProperties = ImageSamplerProperties::Default
		);

		Texture(
			ResourceID id,
			const string& name,
			const ImageDescription& description,
			const ImageSamplerProperties& samplerProperties = ImageSamplerProperties::Default
		);

		Texture(
			ResourceID id,
			const string& name,
			const string& filePath,
			ImageUsageFlags usageFlags,
			const ImageSamplerProperties& samplerProperties = ImageSamplerProperties::Default,
			int channelCount = 4
		);

		~Texture() override;

		DefineResourceType(Texture)

		/// @brief Sets the pixel data for this texture
		/// @param offset The offset in the texture memory to start loading pixel data into
		/// @param size The size of the pixel data being loaded 
		/// @param pixelData The pixel data
		void SetPixels(uint64_t offset, uint64_t size, const void* pixelData);

		/// @brief Sets the properties for sampling this texture
		/// @param repeatMode The repeat mode
		/// @param filterMode The filter mode
		/// @param maxAnisotropy The maximum amount of anisotropy to use
		void SetSamplerProperties(const ImageSamplerProperties& samplerProperties);

		/// @brief Gets the image description of this texture
		/// @return The image description of this texture
		ImageDescription GetDescription() const noexcept;

		const ImageSamplerProperties& GetSamplerProperties() const { return _samplerProperties; }

		/// @brief Gets the image that backs this texture
		/// @return The image that backs this texture
		Ref<Image> GetImage() noexcept { return _image; }

		/// @brief Gets this texture's sampler
		/// @return This texture's sampler
		Ref<ImageSampler> GetSampler() noexcept { return _sampler; }

		/// @brief Loads image data from a file into this texture
		/// @param filePath The path of the image file
		/// @param channelCount The desired number of channels to load
		/// @return True if the image was loaded successfully
		bool LoadFromFile(const string& filePath, int channelCount = 4);

		/// @brief Gets the path to the image that this texture has loaded, if any
		/// @return The path to the image that this texture has loaded, or an empty string if one has not been loaded
		const string& GetImageFilePath() const noexcept { return _imageFilePath; }

	private:
		/// @brief Recreates the internal image using a given description
		/// @param newDescription The new image description
		void RecreateImageFromDescription(const ImageDescription& newDescription);

		/// @brief Recreates the internal sampler
		void RecreateInternalSampler();
	};
}