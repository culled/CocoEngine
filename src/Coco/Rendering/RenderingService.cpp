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
			Render(presenter, _defaultPipeline);
	}

	void RenderingService::Render(GraphicsPresenter* presenter, Ref<RenderPipeline>& pipeline)
	{
		SizeInt size = presenter->GetBackbufferSize();
		Matrix4x4 projectionMat = Matrix4x4::CreatePerspectiveProjection(Math::Deg2Rad(45.0), static_cast<double>(size.Width) / size.Height, 0.1, 100.0);

		// Camera is looking in the -Z axis, so move forward to see our mesh at the world origin
		Matrix4x4 viewMat = Matrix4x4::CreateWithTranslation(Vector3(0, 0, 30));

		// TODO: create this from the scene graph
		Ref<RenderView> view = CreateRef<RenderView>(Vector2Int::Zero, size, pipeline->GetClearColor(), projectionMat, viewMat);

		// Acquire the render context that we'll be using
		RenderContext* renderContext = presenter->GetRenderContext();

		if (renderContext == nullptr)
		{
			LogError(GetLogger(), "Failed to get RenderContext to render presenter");
			return;
		}

		// Actually render with the pipeline
		renderContext->Begin(view, pipeline);
		DoRender(pipeline, renderContext);
		renderContext->End();

		// Submit the render data to the gpu and present
		presenter->Present(renderContext);
	}

	void RenderingService::DoRender(const Ref<RenderPipeline>& pipeline, RenderContext* context)
	{
		context->RestoreViewport();

		// Do render!
		pipeline->Execute(context);
	}
}