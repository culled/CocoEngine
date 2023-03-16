#pragma once

#include <Coco/Core/Resources/Resource.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
	class GraphicsPlatform;

	/// <summary>
	/// A texture that can be used for rendering
	/// </summary>
	class COCOAPI Texture : public Resource
	{
	private:
		GraphicsPlatform* _platform;
		GraphicsResourceRef<Image> _image;
		GraphicsResourceRef<ImageSampler> _sampler;
		uint32_t _refreshCount = 0;
		ImageDescription _description;
		FilterMode _filterMode;
		RepeatMode _repeatMode;
		uint _maxAnisotropy;

	public:
		Texture(
			int width, 
			int height, 
			PixelFormat pixelFormat, 
			ColorSpace colorSpace, 
			ImageUsageFlags usageFlags, 
			FilterMode filterMode,
			RepeatMode repeatMode,
			uint maxAnisotropy,
			GraphicsPlatform* platform);
		virtual ~Texture();

		/// <summary>
		/// Sets the pixel data for this texture
		/// </summary>
		/// <param name="pixelData">The pixel data</param>
		void SetPixels(const void* pixelData) { _image->SetPixelData(pixelData); }

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

	private:
		/// <summary>
		/// Recreates the internal image
		/// </summary>
		void RecreateInternalImage();

		/// <summary>
		/// Recreates the internal sampler
		/// </summary>
		void RecreateInternalSampler();
	};
}