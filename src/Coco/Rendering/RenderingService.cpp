#include "RenderingService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Types/Array.h>
#include "Graphics/RenderContext.h"
#include <limits>

namespace Coco::Rendering
{
	RenderingService::RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams)
	{
		_graphics = GraphicsPlatform::CreatePlatform(this, backendCreateParams);
		CreateDefaultTexture();
	}

	RenderingService::~RenderingService()
	{
		_defaultPipeline.reset();
		_defaultTexture.reset();
		_graphics.reset();
	}

	Logging::Logger* RenderingService::GetLogger() const noexcept
	{
		return Engine::Get()->GetLogger();
	}

	void RenderingService::Start() noexcept
	{}

	void RenderingService::Render(GraphicsPresenter* presenter) noexcept
	{
		if (_defaultPipeline)
		{
			Render(presenter, _defaultPipeline, Engine::Get()->GetApplication()->GetCamera().get());
		}
		else
		{
			LogError(GetLogger(), "Failed to render presenter: No render pipeline was given and no default render pipeline has been set");
		}
	}

	void RenderingService::Render(GraphicsPresenter* presenter, Ref<RenderPipeline> pipeline, CameraComponent* camera) noexcept
	{
		SizeInt size = presenter->GetBackbufferSize();

		// Make sure the camera matches our rendering aspect ratio
		camera->SetAspectRatio(static_cast<double>(size.Width) / size.Height);

		Ref<RenderView> view;
		try
		{
			// TODO: create this from the scene graph
			view = CreateRef<RenderView>(Vector2Int::Zero, size, pipeline->GetClearColor(), camera->GetProjectionMatrix(), camera->GetViewMatrix());
		}
		catch (...)
		{
			LogError(GetLogger(), "Failed to create RenderView");
			return;
		}

		// Acquire the render context that we'll be using
		GraphicsResourceRef<RenderContext> renderContext;
		if (!presenter->GetRenderContext(renderContext))
		{
			LogError(GetLogger(), "Failed to get RenderContext from render presenter");
			return;
		}

		// Actually render with the pipeline
		renderContext->Begin(view, pipeline);
		DoRender(pipeline.get(), renderContext.get());
		renderContext->End();

		// Submit the render data to the gpu and present
		if (!presenter->Present(renderContext.get()))
		{
			LogError(GetLogger(), "Failed to present");
		}
	}

	Ref<Texture> RenderingService::CreateTexture(
		int width,
		int height,
		PixelFormat pixelFormat,
		ColorSpace colorSpace,
		ImageUsageFlags usageFlags,
		FilterMode filterMode,
		RepeatMode repeatMode,
		uint anisotropy)
	{
		return CreateRef<Texture>(width, height, pixelFormat, colorSpace, usageFlags, filterMode, repeatMode, anisotropy, _graphics.get());
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

	void RenderingService::CreateDefaultTexture()
	{
		LogInfo(GetLogger(), "Creating default texture...");

		constexpr int size = 32;
		constexpr int channels = 4;

		_defaultTexture = CreateTexture(
			size, size, 
			PixelFormat::BGRA8, ColorSpace::sRGB, 
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

		_defaultTexture->SetPixels(pixelData.Data());
	}
}