#pragma once

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
		ViewportPanel(SharedRef<ECS::Scene> scene, SelectionContext& selectionContext);
		~ViewportPanel();

		void ChangeScene(SharedRef<ECS::Scene> scene);
		void Draw(const TickInfo& tickInfo);
		void RenderFramebuffer(Rendering::RenderPipeline& pipeline);

	private:
		SelectionContext& _selectionContext;
		SharedRef<ECS::Scene> _scene;
		SharedRef<Rendering::Texture> _framebuffer;
		ECS::Entity _viewportEntity;

		RectInt _viewportRect;
		bool _isOpen;
		bool _isNavigating;

	private:
		void SetupViewportEntity();
		void UpdateProperties();
		void UpdateViewportCamera();
		void UpdateFramebuffers();

		void DrawFramebuffer();
	};
}