#pragma once

#include <Coco/ECS/Entity.h>

using namespace Coco;

class ImGuiLayer
{
private:
	ECS::Entity _cameraEntity;

	int _movingAverageCount;
	int _averageFps;
	double _averageFrameTime;

public:
	ImGuiLayer(const ECS::Entity& cameraEntity);

	void Draw();

private:
	void DrawSettings();
	void DrawRenderStats();
};

