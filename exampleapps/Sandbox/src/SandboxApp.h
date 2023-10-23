#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/AttachmentCache.h>
#include "Rendering/RenderViewProvider3D.h"
#include "Rendering/SceneDataProvider3D.h"
#include "Rendering/RenderViewProvider2D.h"
#include "Rendering/SceneDataProvider2D.h"
#include "ImGui/ImGuiLayer.h"

using namespace Coco;

class SandboxApp : public Application
{
private:
	ManagedRef<TickListener> _tickListener;

	UniqueRef<AttachmentCache> _attachmentCache;

	SharedRef<Rendering::RenderPipeline> _pipeline3D;
	UniqueRef<RenderViewProvider3D> _renderViewProvider3D;
	UniqueRef<SceneDataProvider3D> _sceneDataProvider3D;

	SharedRef<Rendering::RenderPipeline> _pipeline2D;
	UniqueRef<RenderViewProvider2D> _renderViewProvider2D;
	UniqueRef<SceneDataProvider2D> _sceneDataProvider2D;

	UniqueRef<ImGuiLayer> _imGuiLayer;

public:
	SandboxApp();
	~SandboxApp();

protected:
	void Start() final;

private:
	void Tick(const TickInfo& tickInfo);
};

