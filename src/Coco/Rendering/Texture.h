#pragma once

#include "RenderingResource.h"

#include "Graphics/Resources/Image.h"
#include "Graphics/Resources/ImageSampler.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A texture that can be used for rendering
	/// </summary>
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
			uint maxAnisotropy = 16
		);

		Texture(
			const ImageDescription& description,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16
		);

		Texture(
			const string& filePath,
			ImageUsageFlags usageFlags,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16,
			int channelCount = 4
		);

		virtual ~Texture();

		/// <summary>
		/// Sets the pixel data for this texture
		/// </summary>
		/// <param name="offset">The offset in the texture memory to start loading pixel data into</param>
		/// <param name="size">The size of the pixel data being loaded</param>
		/// <param name="pixelData">The pixel data</param>
		void SetPixels(uint64_t offset, uint64_t size, const void* pixelData);

		/// <summary>
		/// Sets the properties for sampling this texture
		/// </summary>
		/// <param name="repeatMode">The repeat mode</param>
		/// <param name="filterMode">The filter mode</param>
		/// <param name="maxAnisotropy">The maximum amount of anisotropy to use</param>
		void SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint maxAnisotropy);

		/// <summary>
		/// Gets the image description of this texture
		/// </summary>
		/// <returns>The image description of this texture</returns>
		ImageDescription GetDescription() const noexcept;

		/// <summary>
		/// Gets the repeat mode for this texture's sampler
		/// </summary>
		/// <returns>This texture's repeat mode</returns>
		RepeatMode GetRepeatMode() const noexcept { return _repeatMode; }

		/// <summary>
		/// Gets the filter mode for this texture's sampler
		/// </summary>
		/// <returns>This texture's filter mode</returns>
		FilterMode GetFilterMode() const noexcept { return _filterMode; }

		/// <summary>
		/// Gets the maximum amount of anisotropy that this texture's sampler will use
		/// </summary>
		/// <returns>The maximum amount of anisotropy that this texture's sampler will use</returns>
		uint GetMaxAnisotropy() const noexcept { return _maxAnisotropy; }

		/// <summary>
		/// Gets the image that backs this texture
		/// </summary>
		/// <returns>The image that backs this texture</returns>
		WeakManagedRef<Image> GetImage() const noexcept { return _image; }

		/// <summary>
		/// Gets this texture's sampler
		/// </summary>
		/// <returns>This texture's sampler</returns>
		WeakManagedRef<ImageSampler> GetSampler() const noexcept { return _sampler; }

		/// <summary>
		/// Loads image data from a file into this texture
		/// </summary>
		/// <param name="filePath">The path of the image file</param>
		/// <param name="channelCount">The desired number of channels to load</param>
		/// <returns>True if the image was loaded successfully</returns>
		bool LoadFromFile(const string& filePath, int channelCount = 4);

		/// <summary>
		/// Gets the path to the image that this texture has loaded, if any
		/// </summary>
		/// <returns>The path to the image that this texture has loaded, or an empty string if one has not been loaded</returns>
		const string& GetImageFilePath() const noexcept { return _imageFilePath; }

	private:
		/// <summary>
		/// Recreates the internal image using a given description
		/// </summary>
		/// <param name="newDescription">The new image description</param>
		void RecreateImageFromDescription(const ImageDescription& newDescription);

		/// <summary>
		/// Recreates the internal sampler
		/// </summary>
		void RecreateInternalSampler();
	};
}