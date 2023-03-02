#include "RenderingService.h"

#include <Coco/Core/Engine.h>
#include "Graphics/RenderContext.h"
#include <limits>

namespace Coco::Rendering
{
	RenderingService::RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams)
	{
		_graphics = GraphicsPlatform::CreatePlatform(this, backendCreateParams);
	}

	RenderingService::~RenderingService()
	{
		_graphics.reset();
	}

	Logging::Logger* RenderingService::GetLogger() const
	{
		return Engine::Get()->GetLogger();
	}

	void RenderingService::Start()
	{}

	void RenderingService::SetDefaultPipeline(Ref<RenderPipeline> pipeline)
	{
		_defaultPipeline = pipeline;
	}

	void RenderingService::Render(GraphicsPresenter* presenter)
	{
		if (_defaultPipeline)
			Render(presenter, _defaultPipeline);
	}

	void RenderingService::Render(GraphicsPresenter* presenter, const Ref<RenderPipeline>& pipeline)
	{
		// TODO: create this from the scene graph
		Ref<RenderView> view = CreateRef<RenderView>(Vector2Int::Zero, presenter->GetBackbufferSize(), pipeline->GetClearColor());

		// Acquire the image that we'll be using
		int backbufferIndex;
		GraphicsPresenterResult result = presenter->AcquireNextBackbuffer(std::numeric_limits<unsigned long long>::max(), backbufferIndex);

		// Early out if the presenter needs to rebuild
		if (result != GraphicsPresenterResult::Success)
			return;

		// Create a render context
		Managed<RenderContext> context = presenter->CreateRenderContext(view, pipeline, backbufferIndex);
		context->Begin();

		// Actually render with the pipeline
		DoRender(pipeline, context.get());

		// TODO: present transition

		// Submit the render data to the gpu and present
		context->End();
		presenter->Present(backbufferIndex);

		context.reset();
		view.reset();
	}

	void RenderingService::DoRender(const Ref<RenderPipeline>& pipeline, RenderContext* context)
	{
		context->RestoreViewport();

		// Do render!
	}
}