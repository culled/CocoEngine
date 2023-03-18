#pragma once

#include <Coco/Core/Resources/Resource.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
	class RenderingService;

	/// <summary>
	/// A texture that can be used for rendering
	/// </summary>
	class COCOAPI Texture : public Resource
	{
	private:
		GraphicsResourceRef<Image> _image = nullptr;
		GraphicsResourceRef<ImageSampler> _sampler = nullptr;
		ImageDescription _description = {};
		FilterMode _filterMode = FilterMode::Linear;
		RepeatMode _repeatMode = RepeatMode::Repeat;
		uint _maxAnisotropy = 0;

	public:
		Texture(
			int width, 
			int height, 
			PixelFormat pixelFormat, 
			ColorSpace colorSpace, 
			ImageUsageFlags usageFlags, 
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16);

		Texture(const ImageDescription& description,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16);

		Texture(const string& filePath,
			ImageUsageFlags usageFlags,
			FilterMode filterMode = FilterMode::Linear,
			RepeatMode repeatMode = RepeatMode::Repeat,
			uint maxAnisotropy = 16,
			int channelCount = 4);

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
		GraphicsResourceRef<Image> GetImage() const noexcept { return _image; }

		/// <summary>
		/// Gets this texture's sampler
		/// </summary>
		/// <returns>This texture's sampler</returns>
		GraphicsResourceRef<ImageSampler> GetSampler() const noexcept { return _sampler; }

		/// <summary>
		/// Loads image data from a file into this texture
		/// </summary>
		/// <param name="filePath">The path of the image file</param>
		/// <param name="channelCount">The desired number of channels to load</param>
		/// <returns>True if the image was loaded successfully</returns>
		bool LoadFromFile(const string& filePath, int channelCount = 4);

	private:
		/// <summary>
		/// Recreates the internal image to use a given description
		/// </summary>
		/// <param name="newDescription">The new image description</param>
		void RecreateFromDescription(const ImageDescription& newDescription);

		/// <summary>
		/// Recreates the internal image
		/// </summary>
		void RecreateInternalImage();

		/// <summary>
		/// Recreates the internal sampler
		/// </summary>
		void RecreateInternalSampler();

		/// <summary>
		/// Ensures that there is an active rendering service and returns it
		/// </summary>
		/// <returns>The active rendering service</returns>
		RenderingService* EnsureRenderingService();
	};
}