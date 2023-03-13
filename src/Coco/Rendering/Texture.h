#pragma once

#include <Coco/Core/Resources/IResource.h>
#include "Graphics/Image.h"
#include "Graphics/ImageSampler.h"
#include "Graphics/GraphicsPlatform.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A texture that can be used for rendering
	/// </summary>
	class COCOAPI Texture : public IResource
	{
	private:
		GraphicsPlatform* _platform;
		uint64_t _id;
		GraphicsResourceRef<Image> _image;
		GraphicsResourceRef<ImageSampler> _sampler;
		uint32_t _refreshCount = 0;
		ImageDescription _description;
		FilterMode _filterMode = FilterMode::Linear;
		RepeatMode _repeatMode = RepeatMode::Repeat;
		uint _anisotropy = 16;

	public:
		Texture(int width, int height, PixelFormat pixelFormat, ColorSpace colorSpace, ImageUsageFlags usageFlags, GraphicsPlatform* platform);
		virtual ~Texture() override;

		virtual uint64_t GetID() const override { return 0; } // TODO

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

		void RecreateInternalSampler();
	};
}