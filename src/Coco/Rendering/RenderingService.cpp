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
		_defaultPipeline.reset();
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
		{
			Render(presenter, _defaultPipeline, Engine::Get()->GetApplication()->GetCamera());
		}
	}

	void RenderingService::Render(GraphicsPresenter* presenter, Ref<RenderPipeline>& pipeline, const Ref<CameraComponent>& camera)
	{
		SizeInt size = presenter->GetBackbufferSize();

		// Make sure the camera matches our rendering aspect ratio
		camera->SetAspectRatio(static_cast<double>(size.Width) / size.Height);

		//Matrix4x4 viewMat = _graphics->AdjustViewMatrix(camera->GetViewMatrix());

		// TODO: create this from the scene graph
		Ref<RenderView> view = CreateRef<RenderView>(Vector2Int::Zero, size, pipeline->GetClearColor(), camera->GetProjectionMatrix(), camera->GetViewMatrix());

		// Acquire the render context that we'll be using
		GraphicsResourceRef<RenderContext> renderContext;

		if (!presenter->GetRenderContext(renderContext))
		{
			LogError(GetLogger(), "Failed to get RenderContext to render presenter");
			return;
		}

		// Actually render with the pipeline
		renderContext->Begin(view, pipeline);
		DoRender(pipeline, renderContext.get());
		renderContext->End();

		// Submit the render data to the gpu and present
		presenter->Present(renderContext);
	}

	Ref<Texture> RenderingService::CreateTexture(int width, int height, PixelFormat pixelFormat, ColorSpace colorSpace, ImageUsageFlags usageFlags)
	{
		return CreateRef<Texture>(width, height, pixelFormat, colorSpace, usageFlags, _graphics.get());
	}

	void RenderingService::DoRender(const Ref<RenderPipeline>& pipeline, RenderContext* context)
	{
		context->RestoreViewport();

		// Do render!
		pipeline->Execute(context);
	}
}