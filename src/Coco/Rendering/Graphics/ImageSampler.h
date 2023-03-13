#pragma once

#include <Coco/Core/Core.h>
#include "GraphicsResource.h"
#include "ImageSamplerTypes.h"

namespace Coco::Rendering
{
	class COCOAPI ImageSampler : public IGraphicsResource
	{
	protected:
		FilterMode FilterMode;
		RepeatMode RepeatMode;
		uint MaxAnisotropy;

	protected:
		ImageSampler(Rendering::FilterMode filterMode, Rendering::RepeatMode repeatMode, uint maxAnisotropy);

	public:
		virtual ~ImageSampler() = default;

		Rendering::FilterMode GetFilterMode() const { return FilterMode; }
		Rendering::RepeatMode GetRepeatMode() const { return RepeatMode; }
	};
}