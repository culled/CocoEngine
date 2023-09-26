#pragma once
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Rendering/Graphics/AttachmentCache.h>

using namespace Coco;
using namespace Coco::Rendering;

class RenderViewProvider2D :
    public RenderViewProvider
{
private:
	AttachmentCache _attachmentCache;

public:
	RenderViewProvider2D();

	UniqueRef<RenderView> CreateRenderView(
		const CompiledRenderPipeline& pipeline,
		uint64 rendererID,
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers) final;
};

