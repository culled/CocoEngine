#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/ECS/Scene.h>
#include <Coco/ECS/Entity.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "../SelectionContext.h"

namespace Coco
{
	class ViewportPanel
	{
	public:
		using ViewportMenuDrawFunction = std::function<void(ViewportPanel& viewport)>;
		using Viewport2DDrawFunction = std::function<bool(ViewportPanel& viewport)>;
		using Viewport3DDrawFunction = std::function<void(ViewportPanel& viewport)>;

	public:
		ViewportPanel(SharedRef<ECS::Scene> scene, SelectionContext& selectionContext, SharedRef<Rendering::RenderPipeline> pipeline);
		~ViewportPanel();

		uint64 GetID() const { return _id; }

		void SetScene(SharedRef<ECS::Scene> scene);
		SharedRef<ECS::Scene> GetScene() const { return _scene; }

		SelectionContext& GetSelectionContext() { return _selectionContext; }
		const RectInt& GetViewportRect() const { return _viewportRect; }
		void GetViewportCameraProjection(Matrix4x4& outViewMatrix, Matrix4x4& outProjectionMatrix) const;
		ECS::Entity GetViewportCameraEntity() { return _viewportEntity; }

		SharedRef<Rendering::RenderPipeline> GetRenderPipeline() { return _pipeline; }

		void Draw(const TickInfo& tickInfo);
		void RenderFramebuffer();

		static void AddMenuHook(const string& path, ViewportMenuDrawFunction drawFunction);
		static void Add2DRenderHook(Viewport2DDrawFunction drawFunction);
		static void Add3DRenderHook(Viewport3DDrawFunction drawFunction);

		void SetOverrideCamera(ECS::Entity& overrideCamera);
		ECS::Entity GetOverrideCamera() { return _overrideCameraEntity; }

	private:
		static std::unordered_map<string, std::vector<ViewportMenuDrawFunction>> _menuHooks;
		static std::vector<Viewport2DDrawFunction> _render2DHooks;
		static std::vector<Viewport3DDrawFunction> _render3DHooks;

		uint64 _id;
		SelectionContext& _selectionContext;
		SharedRef<Rendering::RenderPipeline> _pipeline;
		SharedRef<ECS::Scene> _scene;
		SharedRef<Rendering::Texture> _framebuffer;
		ECS::Entity _viewportEntity;
		ECS::Entity _overrideCameraEntity;

		RectInt _viewportRect;
		bool _isOpen;
		bool _isNavigating;
		bool _isMouseHovingOverGizmo;

	private:
		void SetupViewportEntity();
		void UpdateProperties();
		void UpdateViewportCamera();
		void UpdateFramebuffers();

		void DrawMenu();
		void Draw2DHooks();
		void Draw3DHooks();

		void DrawFramebuffer();
	};
}