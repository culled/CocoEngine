#pragma once

#include <vector>
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
		using Viewport2DDrawFunction = std::function<void(ViewportPanel& viewport)>;

	public:
		ViewportPanel(SharedRef<ECS::Scene> scene, SelectionContext& selectionContext);
		~ViewportPanel();

		void SetScene(SharedRef<ECS::Scene> scene);
		SharedRef<ECS::Scene> GetScene() const { return _scene; }

		SelectionContext& GetSelectionContext() { return _selectionContext; }
		const RectInt& GetViewportRect() const { return _viewportRect; }
		void GetViewportCameraProjection(Matrix4x4& outViewMatrix, Matrix4x4& outProjectionMatrix) const;

		void Draw(const TickInfo& tickInfo);
		void RenderFramebuffer(Rendering::RenderPipeline& pipeline);

		static void Add2DRenderHook(Viewport2DDrawFunction drawFunction);

	private:
		static std::vector<Viewport2DDrawFunction> _render2DHooks;

		SelectionContext& _selectionContext;
		SharedRef<ECS::Scene> _scene;
		SharedRef<Rendering::Texture> _framebuffer;
		ECS::Entity _viewportEntity;

		RectInt _viewportRect;
		bool _isOpen;
		bool _isNavigating;
		bool _isMouseHovingOverGizmo;

	private:
		void SetupViewportEntity();
		void UpdateProperties();
		void UpdateViewportCamera();
		void UpdateFramebuffers();

		void Draw2DHooks();

		void DrawFramebuffer();
	};
}