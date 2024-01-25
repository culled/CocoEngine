#pragma once
#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Scene.h>
#include <Coco/Core/MainLoop/TickInfo.h>

using namespace Coco;
class ECSScene
{
public:
	ECSScene();

	void Update(const TickInfo& tickInfo);
	ECS::Entity GetCameraEntity() { return _camera; }
	SharedRef<ECS::Scene> GetScene() { return _scene; }

private:
	SharedRef<ECS::Scene> _scene;
	ECS::Entity _box;
	ECS::Entity _camera;
};

