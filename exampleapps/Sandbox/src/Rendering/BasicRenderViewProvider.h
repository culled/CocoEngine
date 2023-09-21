#pragma once
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Rendering/Graphics/AttachmentCache.h>
#include <Coco/Core/MainLoop/TickListener.h>

using namespace Coco;
using namespace Coco::Rendering;

class BasicRenderViewProvider :
    public RenderViewProvider
{
private:
	Color _clearColor;
	Transform3D _cameraTransform;
	double _mouseSensitivity = 0.01;
	TickListener _tickListener;
	AttachmentCache _attachmentCache;

public:
	BasicRenderViewProvider();
	~BasicRenderViewProvider();

	UniqueRef<RenderView> CreateRenderView(
		const CompiledRenderPipeline& pipeline,
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers) final;

private:
	void Tick(const TickInfo& tickInfo);
};

