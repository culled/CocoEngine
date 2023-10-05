#pragma once

#include <Coco/Rendering/Graphics/AttachmentCache.h>
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include "../../Entity.h"

using namespace Coco::Rendering;

namespace Coco
{
	struct Transform3D;
}

namespace Coco::Rendering
{
	class RenderPipeline;
}

namespace Coco::ECS
{
	struct CameraComponent;

	/// @brief Sets up a render view for rendering from a camera entity's perspective
	class CameraRenderViewProvider :
		public RenderViewProvider
	{
	private:
		const Entity& _entity;

	public:
		CameraRenderViewProvider(const Entity& camera);

		// Inherited via RenderViewProvider
		void SetupRenderView(
			RenderView& renderView,
			const CompiledRenderPipeline& pipeline,
			uint64 rendererID,
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) override;
	};

	/// @brief A system that manages cameras
	class CameraSystem
	{
		friend class CameraRenderViewProvider;

	private:
		static AttachmentCache _sAttachmentCache;

	public:
		/// @brief Renders the given camera entity
		/// @param camera The entity with at least a CameraComponent
		/// @param framebuffers The framebuffers to render to
		/// @param pipeline The pipeline to use while rendering
		static void Render(const Entity& camera, std::span<Ref<Image>> framebuffers, RenderPipeline& pipeline);
	};
}