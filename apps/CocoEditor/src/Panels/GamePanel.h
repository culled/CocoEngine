#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/ECS/Scene.h>

namespace Coco::Rendering
{
	class RenderPipeline;
}

namespace Coco
{
	class GamePanel
	{
	private:
		RectInt _viewportRect;
		SharedRef<Rendering::Texture> _viewportTexture;
		SharedRef<ECS::Scene> _currentScene;
		bool _isOpen;
		bool _shouldFocus;

	public:
		GamePanel(SharedRef<ECS::Scene> scene);
		~GamePanel();

		void SetCurrentScene(SharedRef<ECS::Scene> scene);

		void Update(const TickInfo& tickInfo);
		void Render(Rendering::RenderPipeline& pipeline);

		void Focus();

	private:
		void UpdateWindowSettings();
		void DrawViewportImage();
		void EnsureViewportTexture();
	};
}