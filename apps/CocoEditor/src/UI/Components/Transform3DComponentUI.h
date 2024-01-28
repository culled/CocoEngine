#pragma once

#include <unordered_map>
#include <Coco/ECS/Components/Transform3DComponent.h>

namespace ImGuizmo
{
	enum OPERATION;
	enum MODE;
}

namespace Coco
{
	class ViewportPanel;

	struct ViewportTransformSettings
	{
		float SnapIncrementMove;
		float SnapIncrementRotate;
		float SnapIncrementScale;
		bool EnableSnap;
		ImGuizmo::OPERATION CurrentOperation;
		ImGuizmo::MODE CurrentMode;

		ViewportTransformSettings();
	};

	class Transform3DComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::Transform3DComponent& transform);
		static bool DrawViewport2D(ViewportPanel& viewport);
		static void DrawViewportMenu(ViewportPanel& viewport);
		static void DrawViewportSnapSettingsMenu(ViewportPanel& viewport);

	private:
		static std::unordered_map<uint64, ViewportTransformSettings> _settings;
	};
}