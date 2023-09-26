#pragma once

#include "../Rendering/RenderViewProvider3D.h"

class ImGuiLayer
{
private:
	RenderViewProvider3D& _viewProvider3D;

public:
	ImGuiLayer(RenderViewProvider3D& viewProvider3D);

	void Draw();
	void DrawPostRender();
};

