#pragma once

#include <Coco/Rendering/RenderingResource.h>

#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	/// @brief A sampler for an image
	class COCOAPI ImageSampler : public RenderingResource
	{
	protected:
		ImageSamplerProperties _properties;

	public:
		ImageSampler(
			ResourceID id,
			const string& name,
			uint64_t lifetime,
			const ImageSamplerProperties& properties
		) noexcept;

		virtual ~ImageSampler() = default;

		ImageSampler(const ImageSampler&) = delete;
		ImageSampler(ImageSampler&&) = delete;

		ImageSampler& operator=(const ImageSampler&) = delete;
		ImageSampler& operator=(ImageSampler&&) = delete;

		const ImageSamplerProperties& GetProperties() const noexcept { return _properties; }
	};
}