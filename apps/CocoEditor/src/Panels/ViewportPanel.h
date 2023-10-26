#pragma once

#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/ECS/Scene.h>

using namespace Coco::Rendering;
using namespace Coco::ECS;

namespace Coco::Rendering
{
	class RenderPipeline;
	class SceneDataProvider;
	struct GlobalShaderUniformLayout;
}

namespace ImGuizmo
{
	enum OPERATION;
	enum MODE;
}

namespace Coco
{
	class SelectionContext;

	class ViewportPanel :
		public RenderViewProvider
	{
	public:
		Event<const ViewportPanel&> OnClosed;

	private:
		static const double _sMinMoveSpeed;
		static const double _sMaxMoveSpeed;
		static const double _sCameraPreviewSizePercentage;

		SelectionContext& _selection;
		string _name;
		bool _collapsed;
		SharedRef<Scene> _currentScene;

		MSAASamples _sampleCount;
		Transform3D _cameraTransform;
		ECS::CameraComponent _cameraComponent;

		double _lookSensitivity;
		double _moveSpeed;
		double _scrollDistance;
		bool _isFlying;
		bool _isMouseHovering;
		bool _isMouseHoveringGizmo;
		bool _isFocused;
		bool _showCameraPreview;
		bool _previewCameraFullscreen;
		SizeInt _cameraPreviewSize;
		RectInt _viewportRect;

		ImGuizmo::OPERATION _currentTransformOperation;

		ManagedRef<Texture> _cameraPreviewTexture;
		ManagedRef<Texture> _viewportTexture;
		ManagedRef<Texture> _viewportPickingTexture;

	public:
		ViewportPanel(const char* name, SharedRef<Scene> scene);
		~ViewportPanel();

		// Inherited via RenderViewProvider
		void SetupRenderView(
			RenderView& renderView, 
			const CompiledRenderPipeline& pipeline, 
			uint64 rendererID, 
			const SizeInt& backbufferSize, 
			std::span<Ref<Image>> backbuffers) override;

		void SetCurrentScene(SharedRef<Scene> scene);

		void Update(const TickInfo& tickInfo);
		void Render(RenderPipeline& pipeline);

	private:
		void EnsureTexture(const SizeInt& size, ManagedRef<Texture>& texture);
		void EnsurePickingTexture(const SizeInt& size);

		void UpdateWindowSettings();
		void UpdateCamera(const TickInfo& tickInfo);

		void DrawSelectedEntity();
		
		void ShowCameraStatsWindow();

		void ShowCameraPreview();

		void PickEntity();
	};
}