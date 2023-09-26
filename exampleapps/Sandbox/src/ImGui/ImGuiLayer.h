#pragma once

#include "../Rendering/RenderViewProvider3D.h"

class ImGuiLayer
{
private:
	RenderViewProvider3D& _viewProvider3D;

	int _movingAverageCount;
	int _averageFps;
	double _averageFrameTime;

public:
	ImGuiLayer(RenderViewProvider3D& viewProvider3D);

	void Draw();
	void DrawPostRender();
};

