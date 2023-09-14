#include "Renderpch.h"
#include "RenderService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory)
	{
		_platform = platformFactory.Create();
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}

	void RenderService::Render(GraphicsPresenter& presenter, RenderPipeline& pipeline)
	{
		RenderContext* context;
		Image* backbuffer;

		if (!presenter.PrepareForRender(context, backbuffer))
		{
			CocoError("Failed to prepare presenter for rendering")
			return;
		}

		// TODO: get RenderView and other rendering-related items from scene
		SizeInt size = presenter.GetFramebufferSize();
		RectInt viewport(Vector2Int::Zero, size);
		std::vector<RenderTarget> rts{ RenderTarget(backbuffer, Color(0.1, 0.2, 0.3, 1.0)) };

		RenderView view(viewport, viewport, rts);

		ExecuteRender(*context, pipeline, view);

		if (!presenter.Present(*context))
		{
			CocoError("Failed to present rendered image")
		}
	}

	void RenderService::ExecuteRender(RenderContext& context, RenderPipeline& pipeline, RenderView& renderView)
	{
		if (!pipeline.Compile())
			return;

		CompiledRenderPipeline compiledPipeline = pipeline.GetCompiledPipeline();

		try
		{
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				if (it == compiledPipeline.RenderPasses.begin())
				{
					if (!context.Begin(renderView, compiledPipeline))
						return;
				}
				else
				{
					if (!context.BeginNextPass())
						break;
				}

				it->Pass->Execute(context, renderView);
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error during render: {}", ex.what())
		}

		context.End();
	}
}