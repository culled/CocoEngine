#pragma once

#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include "../../Entity.h"

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
		/// @param camera The entity with at least a CameraComponent
		/// @param framebuffers The framebuffers to render to
		/// @param pipeline The pipeline to use while rendering
		static void Render(
			const CameraComponent& camera,
			std::span<Ref<Image>> framebuffers, 
			RenderPipeline& pipeline, 
			std::optional<GlobalShaderUniformLayout> layoutOverride = std::optional<GlobalShaderUniformLayout>());

		/// @brief Renders the given camera entity
		/// @param camera The entity with at least a CameraComponent
		/// @param presenter The GraphicsPresenter to render to
		/// @param pipeline The pipeline to use while rendering
		static void Render(
			const CameraComponent& camera,
			Ref<GraphicsPresenter> presenter, 
			RenderPipeline& pipeline,
			std::optional<GlobalShaderUniformLayout> layoutOverride = std::optional<GlobalShaderUniformLayout>());
	};
}