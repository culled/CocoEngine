#pragma once
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Rendering/Graphics/AttachmentCache.h>
#include <Coco/Core/MainLoop/TickListener.h>

using namespace Coco;
using namespace Coco::Rendering;

class RenderViewProvider3D :
    public RenderViewProvider
{
private:
	AttachmentCache& _attachmentCache;
	Color _clearColor;
	MSAASamples _msaaSamples;
	Transform3D _cameraTransform;
	double _mouseSensitivity;
	TickListener _tickListener;

public:
	RenderViewProvider3D(AttachmentCache& attachmentCache);
	~RenderViewProvider3D();

	void SetupRenderView(
		RenderView& renderView,
		const CompiledRenderPipeline& pipeline,
		uint64 rendererID,
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers) final;

	void SetMSAASamples(MSAASamples samples);
	MSAASamples GetMSAASamples() const { return _msaaSamples; }

private:
	void Tick(const TickInfo& tickInfo);
};

