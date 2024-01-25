#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/BuiltIn/BuiltInRenderViewDataProvider.h>

#include "Rendering/SceneProvider3D.h"
#include "ECS/ECSScene.h"

using namespace Coco;

class SandboxApp : public Application
{
public:
	SandboxApp();
	~SandboxApp();

protected:
	void Start() final;

private:
	ManagedRef<TickListener> _tickListener;
	ManagedRef<TickListener> _renderTickListener;
	SharedRef<Rendering::RenderPipeline> _pipeline3D;
	SharedRef<Rendering::RenderPipeline> _pipelineECS;
	SharedRef<Rendering::BuiltInRenderViewDataProvider> _renderViewProvider3D;
	UniqueRef<SceneProvider3D> _sceneProvider3D;
	UniqueRef<ECSScene> _ecsScene;

private:
	void InitializeServices();
	void InitializeResources();

	void Tick(const TickInfo& tickInfo);
	void RenderTick(const TickInfo& tickInfo);
};

