#pragma once

#include "../Rendering/RenderViewProvider3D.h"
#include "../Rendering/SceneDataProvider3D.h"

class ImGuiLayer
{
private:
	RenderViewProvider3D& _viewProvider3D;
	SceneDataProvider3D& _sceneProvider3D;

	int _movingAverageCount;
	int _averageFps;
	double _averageFrameTime;

public:
	ImGuiLayer(RenderViewProvider3D& viewProvider3D, SceneDataProvider3D& sceneProvider3D);

	void Draw();
	void DrawPostRender();
};

