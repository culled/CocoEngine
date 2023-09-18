#pragma once

#include "../Graphics/RenderView.h"
#include "../Pipeline/RenderPipeline.h"

namespace Coco::Rendering
{
	class RenderViewProvider
	{
	public:
		virtual UniqueRef<RenderView> CreateRenderView(
			const RenderPipeline& pipeline, 
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) = 0;
	};
}