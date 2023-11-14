#pragma once
#include <Coco/Rendering/Providers/RenderViewProvider.h>

using namespace Coco;
using namespace Coco::Rendering;

class RenderViewProvider2D :
    public RenderViewProvider
{
public:
	void SetupRenderView(
		RenderView& renderView,
		const CompiledRenderPipeline& pipeline,
		uint64 rendererID,
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers) final;
};

