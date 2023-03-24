#pragma once

#include "GraphicsResource.h"

#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	/// @brief A sampler for an image
	class COCOAPI ImageSampler : public IGraphicsResource
	{
	protected:
		/// @brief The filter mode
		FilterMode FilterMode;

		/// @brief The repeat mode
		RepeatMode RepeatMode;

		/// @brief The maximum amount of anisotropy to use
		uint MaxAnisotropy;

	protected:
		ImageSampler(Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy) noexcept;

	public:
		virtual ~ImageSampler() = default;

		ImageSampler(const ImageSampler&) = delete;
		ImageSampler(ImageSampler&&) = delete;

		ImageSampler& operator=(const ImageSampler&) = delete;
		ImageSampler& operator=(ImageSampler&&) = delete;

		/// @brief Gets this sampler's filter mode
		/// @return The filter mode
		constexpr Rendering::FilterMode GetFilterMode() const noexcept { return FilterMode; }

		/// @brief Gets this sampler's repeat mode
		/// @return The repeat mode
		constexpr Rendering::RepeatMode GetRepeatMode() const noexcept { return RepeatMode; }

		/// @brief Gets this sampler's maximum anisotropy
		/// @return The maximum anisotropy
		constexpr uint GetMaxAnisotropy() const noexcept { return MaxAnisotropy; }
	};
}