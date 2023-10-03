#pragma once

#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Rendering/Graphics/AttachmentCache.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/MainLoop/TickListener.h>

using namespace Coco::Rendering;

namespace Coco::Rendering
{
	class RenderPipeline;
	class SceneDataProvider;
	struct GlobalShaderUniformLayout;
}

namespace Coco
{
	class ViewportPanel :
		public RenderViewProvider
	{
	public:
		Event<const ViewportPanel&> OnClosed;

	private:
		string _name;
		ManagedRef<TickListener> _updateTickListener;
		bool _collapsed;
		Color _clearColor;
		MSAASamples _sampleCount;
		double _verticalFOV;
		Transform3D _cameraTransform;
		double _lookSensitivity;
		double _moveSpeed;
		bool _isFlying;
		UniqueRef<AttachmentCache> _attachmentCache;
		ManagedRef<Texture> _viewportTexture;

	public:
		ViewportPanel(const char* name);
		~ViewportPanel();

		// Inherited via RenderViewProvider
		void SetupRenderView(
			RenderView& renderView, 
			const CompiledRenderPipeline& pipeline, 
			uint64 rendererID, 
			const SizeInt& backbufferSize, 
			std::span<Ref<Image>> backbuffers) override;

		void Render(RenderPipeline& pipeline,
			std::span<SceneDataProvider*> sceneDataProviders);

	private:
		void Update(const TickInfo& tickInfo);
		void EnsureViewportTexture(const SizeInt& size);

		void UpdateCamera(const TickInfo& tickInfo);
	};
}