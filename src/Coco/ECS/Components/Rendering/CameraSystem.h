#pragma once

#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include "../../Entity.h"
#include "../../Scene.h"

using namespace Coco::Rendering;

namespace Coco::Rendering
{
	class RenderPipeline;
	class GraphicsPresenter;
}

namespace Coco::ECS
{
	class CameraComponent;

	/// @brief Sets up a render view for rendering from a camera entity's perspective
	class CameraRenderViewProvider :
		public RenderViewProvider
	{
	private:
		const Entity _entity;
		std::optional<GlobalShaderUniformLayout> _layoutOverride;

	public:
		CameraRenderViewProvider(const Entity& camera, std::optional<GlobalShaderUniformLayout> layoutOverride);

		// Inherited via RenderViewProvider
		void SetupRenderView(
			RenderView& renderView,
			const CompiledRenderPipeline& pipeline,
			uint64 rendererID,
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) override;
	};

	/// @brief A system that renders CameraComponents
	class CameraSystem
	{
		friend class CameraRenderViewProvider;

	public:
		/// @brief Renders the given camera entity
        /// @param rendererID The ID of the renderer
		/// @param camera The entity with at least a CameraComponent
		/// @param framebuffers The framebuffers to render to
		/// @param pipeline The pipeline to use while rendering
		/// @param layoutOverride The global shader uniform layout override (optional)
		static void Render(
			uint64 rendererID,
			const CameraComponent& camera,
			std::span<Ref<Image>> framebuffers, 
			RenderPipeline& pipeline, 
			std::optional<GlobalShaderUniformLayout> layoutOverride = std::optional<GlobalShaderUniformLayout>());

		/// @brief Renders the given camera entity
		/// @param camera The entity with at least a CameraComponent
		/// @param presenter The GraphicsPresenter to render to
		/// @param pipeline The pipeline to use while rendering
		/// @param layoutOverride The global shader uniform layout override (optional)
		static void Render(
			const CameraComponent& camera,
			Ref<GraphicsPresenter> presenter, 
			RenderPipeline& pipeline,
			std::optional<GlobalShaderUniformLayout> layoutOverride = std::optional<GlobalShaderUniformLayout>());

		/// @brief Renders a scene from an active camera in that scene, if there is one
		/// @param rendererID The ID of the renderer
		/// @param scene The scene
		/// @param framebuffers The framebuffers to render to
		/// @param pipeline The pipeline to use while rendering
		/// @param layoutOverride The global shader uniform layout override (optional)
		/// @return True if the scene was rendered
		static bool RenderScene(
			uint64 rendererID,
			SharedRef<Scene> scene,
			std::span<Ref<Image>> framebuffers,
			RenderPipeline& pipeline,
			std::optional<GlobalShaderUniformLayout> layoutOverride = std::optional<GlobalShaderUniformLayout>());

		/// @brief Tries to find an active camera in the given scene
		/// @param scene The scene
		/// @param outActiveCamera Will be set to the active camera
		/// @return True if an active camera was found
		static bool TryGetActiveCamera(SharedRef<Scene> scene, CameraComponent*& outActiveCamera);
	};
}