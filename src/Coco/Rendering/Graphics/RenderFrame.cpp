#include "Renderpch.h"
#include "RenderFrame.h"
#include "../Providers/RenderViewDataProvider.h"
#include "../Providers/SceneDataProvider.h"
#include "RenderView.h"

namespace Coco::Rendering
{
	SharedRef<RenderView> RenderFrame::CreateRenderView(
		uint64 rendererID,
		std::span<Ref<Image>> framebuffers,
		const SizeInt& framebufferSize,
		const CompiledRenderPipeline& compiledPipeline,
		RenderViewDataProvider& renderViewDataProvider,
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		SharedRef<RenderView> renderView = CreateSharedRef<RenderView>(*this);
		renderViewDataProvider.SetupRenderView(*renderView, compiledPipeline, rendererID, framebufferSize, framebuffers);

		// Get the scene data
		for (SceneDataProvider* provider : sceneDataProviders)
		{
			if (!provider)
				continue;

			provider->GatherSceneData(*renderView);
		}

		return renderView;
	}
}