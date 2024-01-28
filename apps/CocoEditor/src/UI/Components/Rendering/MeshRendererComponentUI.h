#pragma once

#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>

namespace Coco
{
	class ViewportPanel;

	class MeshRendererComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::MeshRendererComponent& renderer);
		static void DrawViewport3D(ViewportPanel& viewport);
	};
}