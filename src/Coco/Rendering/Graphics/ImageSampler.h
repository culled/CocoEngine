#pragma once

#include <Coco/Core/Core.h>
#include "GraphicsResource.h"
#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A sampler for an image
	/// </summary>
	class COCOAPI ImageSampler : public IGraphicsResource
	{
	protected:
		FilterMode FilterMode;
		RepeatMode RepeatMode;
		uint MaxAnisotropy;

	protected:
		ImageSampler(Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy) noexcept;

	public:
		virtual ~ImageSampler() = default;

		ImageSampler(const ImageSampler&) = delete;
		ImageSampler(ImageSampler&&) = delete;

		ImageSampler& operator=(const ImageSampler&) = delete;
		ImageSampler& operator=(ImageSampler&&) = delete;

		/// <summary>
		/// Gets this sampler's filter mode
		/// </summary>
		/// <returns>The filter mode</returns>
		Rendering::FilterMode GetFilterMode() const noexcept { return FilterMode; }

		/// <summary>
		/// Gets this sampler's repeat mode
		/// </summary>
		/// <returns>The repeat mode</returns>
		Rendering::RepeatMode GetRepeatMode() const noexcept { return RepeatMode; }

		/// <summary>
		/// Gets this sampler's maximum anisotropy
		/// </summary>
		/// <returns>The maximum anisotropy</returns>
		uint GetMaxAnisotropy() const noexcept { return MaxAnisotropy; }
	};
}