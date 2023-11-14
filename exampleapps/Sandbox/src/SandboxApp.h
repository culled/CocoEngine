#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "Rendering/RenderViewProvider2D.h"
#include "Rendering/SceneDataProvider2D.h"
#include "ImGui/ImGuiLayer.h"
#include <Coco/ECS/Scene.h>
#include <Coco/ECS/Entity.h>

using namespace Coco;

class SandboxApp : public Application
{
private:
	ManagedRef<TickListener> _tickListener;

	SharedRef<Rendering::RenderPipeline> _pipeline3D;

	SharedRef<Rendering::RenderPipeline> _pipeline2D;
	UniqueRef<RenderViewProvider2D> _renderViewProvider2D;
	UniqueRef<SceneDataProvider2D> _sceneDataProvider2D;

	UniqueRef<ImGuiLayer> _imGuiLayer;

	SharedRef<ECS::Scene> _scene;
	ECS::Entity _cameraEntity;

public:
	SandboxApp();
	~SandboxApp();

protected:
	void Start() final;

private:
	void Tick(const TickInfo& tickInfo);
};

