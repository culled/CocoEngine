#include "RenderingService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Types/Array.h>
#include "Graphics/RenderContext.h"
#include "Texture.h"
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

namespace Coco::Rendering
{
	RenderingService* RenderingService::s_instance = nullptr;

	RenderingService::RenderingService(Coco::Engine* engine, const GraphicsPlatformCreationParameters& backendCreateParams) : EngineService(engine)
	{
		s_instance = this;
		_graphics = GraphicsPlatform::CreatePlatform(this, backendCreateParams);
		
		// Create default textures
		CreateDefaultDiffuseTexture();
		CreateDefaultCheckerTexture();

		RegisterTickListener(this, &RenderingService::Tick, TickPriority);
	}

	RenderingService::~RenderingService()
	{
		_defaultPipeline.reset();
		_defaultDiffuseTexture.reset();
		_defaultCheckerTexture.reset();
		_graphics.reset();
	}

	void RenderingService::Render(GraphicsPresenter* presenter)
	{
		if (_defaultPipeline)
		{
			Render(presenter, _defaultPipeline, this->Engine->GetApplication()->GetCamera().get());
		}
		else
		{
			LogError(GetLogger(), "Failed to render presenter: No render pipeline was given and no default render pipeline has been set");
		}
	}

	void RenderingService::Render(GraphicsPresenter* presenter, Ref<RenderPipeline> pipeline, CameraComponent* camera)
	{
		SizeInt size = presenter->GetBackbufferSize();

		// Make sure the camera matches our rendering aspect ratio
		camera->SetAspectRatio(static_cast<double>(size.Width) / size.Height);

		// TODO: create this from the scene graph
		Ref<RenderView> view = CreateRef<RenderView>(Vector2Int::Zero, size, pipeline->GetClearColor(), camera->GetProjectionMatrix(), camera->GetViewMatrix());

		// Acquire the render context that we'll be using
		RenderContext* renderContext;
		if (!presenter->GetRenderContext(renderContext))
		{
			LogError(GetLogger(), "Failed to get RenderContext from render presenter");
			return;
		}

		// Actually render with the pipeline
		renderContext->Begin(view, pipeline);
		DoRender(pipeline.get(), renderContext);
		renderContext->End();

		// Submit the render data to the gpu and present
		if (!presenter->Present(renderContext))
		{
			LogError(GetLogger(), "Failed to present");
		}
	}

	void RenderingService::DoRender(RenderPipeline* pipeline, RenderContext* context) noexcept
	{
		try
		{
			context->RestoreViewport();

			// Do render!
			pipeline->Execute(context);
		}
		catch (const Exception& ex)
		{
			LogError(GetLogger(), FormattedString("Rendering failed: {}", ex.what()));
		}
	}

	void RenderingService::CreateDefaultDiffuseTexture()
	{
		LogInfo(GetLogger(), "Creating default diffuse texture...");

		constexpr int size = 32;
		constexpr int channels = 4;

		_defaultDiffuseTexture = CreateRef<Texture>(
			size, size,
			PixelFormat::RGBA8, ColorSpace::sRGB,
			ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled);

		List<uint8_t> pixelData(size * size * channels);

		for (uint64_t i = 0; i < pixelData.Count(); i++)
		{
			pixelData[i] = 255;
		}

		_defaultDiffuseTexture->SetPixels(0, pixelData.Count(), pixelData.Data());
	}

	void RenderingService::CreateDefaultCheckerTexture()
	{
		LogInfo(GetLogger(), "Creating default checker texture...");

		constexpr int size = 32;
		constexpr int channels = 4;

		_defaultCheckerTexture = CreateRef<Texture>(
			size, size,
			PixelFormat::RGBA8, ColorSpace::sRGB,
			ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled,
			FilterMode::Nearest);

		List<uint8_t> pixelData(size * size * channels);

		for (size_t x = 0; x < size; x++)
		{
			for (size_t y = 0; y < size; y++)
			{
				const size_t baseIndex = ((x * size) + y) * channels;
				pixelData[baseIndex + 0] = 255;
				pixelData[baseIndex + 1] = 255;
				pixelData[baseIndex + 2] = 255;
				pixelData[baseIndex + 3] = 255;

				if (((x % 2) && (y % 2)) || !(y % 2))
				{
					pixelData[baseIndex + 1] = 0; // Green = 0
					pixelData[baseIndex + 2] = 0; // Red = 0
				}
			}
		}

		_defaultCheckerTexture->SetPixels(0, pixelData.Count(), pixelData.Data());
	}

	void RenderingService::Tick(double deltaTime)
	{
		_timeSinceLastPurge += deltaTime;

		if (_timeSinceLastPurge > PurgeFrequency)
		{
			_graphics->GetDevice()->PurgeUnusedResources();
			_timeSinceLastPurge = 0.0;
		}
	}
}