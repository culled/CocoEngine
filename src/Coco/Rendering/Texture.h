#pragma once

#include "RenderingResource.h"

#include "Graphics/Resources/Image.h"
#include "Graphics/Resources/ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief A texture that can be used for rendering images
	class COCOAPI Texture : public RenderingResource
	{
	private:
		WeakManagedRef<Image> _image;
		ImageUsageFlags _usageFlags;
		WeakManagedRef<ImageSampler> _sampler;
		FilterMode _filterMode = FilterMode::Linear;
		RepeatMode _repeatMode = RepeatMode::Repeat;
		uint _maxAnisotropy = 0;
		string _imageFilePath;

	public:
		Texture(
			int width, 
			int height, 
			PixelFormat pixelFormat, 
			ColorSpace colorSpace, 
			ImageUsageFlags usageFlags, 
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16,
			const string& name = ""
		);

		Texture(
			const ImageDescription& description,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16,
			const string& name = ""
		);

		Texture(
			const string& filePath,
			ImageUsageFlags usageFlags,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16,
			int channelCount = 4,
			const string& name = ""
		);

		virtual ~Texture();

		/// @brief Sets the pixel data for this texture
		/// @param offset The offset in the texture memory to start loading pixel data into
		/// @param size The size of the pixel data being loaded 
		/// @param pixelData The pixel data
		void SetPixels(uint64_t offset, uint64_t size, const void* pixelData);

		/// @brief Sets the properties for sampling this texture
		/// @param repeatMode The repeat mode
		/// @param filterMode The filter mode
		/// @param maxAnisotropy The maximum amount of anisotropy to use
		void SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint maxAnisotropy);

		/// @brief Gets the image description of this texture
		/// @return The image description of this texture
		ImageDescription GetDescription() const noexcept;

		/// @brief Gets the repeat mode for this texture's sampler
		/// @return This texture's repeat mode
		constexpr RepeatMode GetRepeatMode() const noexcept { return _repeatMode; }

		/// @brief Gets the filter mode for this texture's sampler
		/// @return This texture's filter mode
		constexpr FilterMode GetFilterMode() const noexcept { return _filterMode; }
		
		/// @brief Gets the maximum amount of anisotropy that this texture's sampler will use
		/// @return The maximum amount of anisotropy that this texture's sampler will use
		constexpr uint GetMaxAnisotropy() const noexcept { return _maxAnisotropy; }

		/// @brief Gets the image that backs this texture
		/// @return The image that backs this texture
		WeakManagedRef<Image> GetImage() const noexcept { return _image; }

		/// @brief Gets this texture's sampler
		/// @return This texture's sampler
		WeakManagedRef<ImageSampler> GetSampler() const noexcept { return _sampler; }

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