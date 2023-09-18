#pragma once
#include <Coco/Rendering/Providers/RenderViewProvider.h>

using namespace Coco;
using namespace Coco::Rendering;

class BasicRenderViewProvider :
    public RenderViewProvider
{
private:
	Color _clearColor;

public:
	BasicRenderViewProvider();

	UniqueRef<RenderView> CreateRenderView(
		const RenderPipeline& pipeline,
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers) final;
};

