#pragma once

#include <unordered_map>

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/Rendering/Texture.h>

namespace Coco
{
	class ViewportPanel;

	struct ViewportCameraComponentSettings
	{
		float CameraPreviewScale;
		SharedRef<Rendering::Texture> CameraPreviewTexture;
		ECS::Entity FullscreenPreviewEntity;

		ViewportCameraComponentSettings(uint64 viewportID);
	};

	class CameraComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::CameraComponent& camera);
		static bool DrawViewport2D(ViewportPanel& viewport);
		static void DrawViewport3D(ViewportPanel& viewport);

	private:
		static std::unordered_map<uint64, ViewportCameraComponentSettings> _settings;

	private:
		static void EnsureCameraPreviewTexture(uint64 viewportID, ViewportCameraComponentSettings& settings, const SizeInt& previewSize);
	};
}
