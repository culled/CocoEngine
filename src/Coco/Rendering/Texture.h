#pragma once

#include <Coco/Core/Resources/IResource.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"

namespace Coco::Rendering
{
	class GraphicsPlatform;

	/// <summary>
	/// A texture that can be used for rendering
	/// </summary>
	class COCOAPI Texture : public IResource
	{
	private:
		GraphicsPlatform* _platform;
		GraphicsResourceRef<Image> _image;
		GraphicsResourceRef<ImageSampler> _sampler;
		uint32_t _refreshCount = 0;
		ImageDescription _description;
		FilterMode _filterMode;
		RepeatMode _repeatMode;
		uint _anisotropy;

	public:
		Texture(
			int width, 
			int height, 
			PixelFormat pixelFormat, 
			ColorSpace colorSpace, 
			ImageUsageFlags usageFlags, 
			FilterMode filterMode,
			RepeatMode repeatMode,
			uint anisotropy,
			GraphicsPlatform* platform);
		virtual ~Texture() override;

		virtual ResourceID GetID() const override { return 0; } // TODO

		/// <summary>
		/// Sets the pixel data for this texture
		/// </summary>
		/// <param name="pixelData">The pixel data</param>
		void SetPixels(const void* pixelData);

		void SetSamplerProperties(RepeatMode repeatMode, FilterMode filterMode, uint anisotropy);
		RepeatMode GetRepeatMode() const { return _repeatMode; }
		FilterMode GetFilterMode() const { return _filterMode; }
		uint GetAnisotropy() const { return _anisotropy; }

		GraphicsResourceRef<Image> GetImage() const { return _image; }
		GraphicsResourceRef<ImageSampler> GetSampler() const { return _sampler; }

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