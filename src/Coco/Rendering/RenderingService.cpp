#include "RenderingService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Types/Array.h>
#include "Graphics/GraphicsDevice.h"
#include "Serializers/TextureSerializer.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/MaterialSerializer.h"
#include "Texture.h"

namespace Coco::Rendering
{
	RenderingService::RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams) : EngineService()
	{
		this->SetSingleton(this);

		_graphics = GraphicsPlatform::CreatePlatform(backendCreateParams);
		
		// Create default textures
		CreateDefaultDiffuseTexture();
		CreateDefaultCheckerTexture();
		CreateDefaultNormalTexture();

		ResourceLibrary* resourceLibrary = Engine::Get()->GetResourceLibrary();
		resourceLibrary->CreateSerializer<TextureSerializer>();
		resourceLibrary->CreateSerializer<ShaderSerializer>();
		resourceLibrary->CreateSerializer<MaterialSerializer>();
	}

	RenderingService::~RenderingService()
	{
		_graphics.Reset();
	}

	void RenderingService::AddSceneDataProvider(Ref<ISceneDataProvider> provider)
	{
		if (_sceneDataProviders.Contains([provider](const Ref<ISceneDataProvider>& other) { return provider.Get() == other.Get(); }))
			return;

		_sceneDataProviders.Add(provider);
	}

	void RenderingService::RemoveSceneDataProvider(Ref<ISceneDataProvider> provider)
	{
		_sceneDataProviders.RemoveAll([provider](const Ref<ISceneDataProvider>& other) { return provider.Get() == other.Get(); });
	}

	void RenderingService::Render(
		Ref<GraphicsPresenter> presenter,
		ICameraDataProvider& cameraDataProvider)
	{
		Render(presenter, cameraDataProvider, _sceneDataProviders);
	}

	void RenderingService::Render(Ref<GraphicsPresenter> presenter, ICameraDataProvider& cameraDataProvider, List<Ref<ISceneDataProvider>> sceneDataProviders)
	{
		if (_defaultPipeline)
		{
			Render(presenter, _defaultPipeline, cameraDataProvider, sceneDataProviders);
		}
		else
		{
			LogError(GetLogger(), "Failed to render: No render pipeline was given and no default render pipeline has been set");
		}
	}

	void RenderingService::Render(Ref<GraphicsPresenter> presenter, Ref<RenderPipeline> pipeline, ICameraDataProvider& cameraDataProvider)
	{
		Render(presenter, pipeline, cameraDataProvider, _sceneDataProviders);
	}

	void RenderingService::Render(
		Ref<GraphicsPresenter> presenter,
		Ref<RenderPipeline> pipeline,
		ICameraDataProvider& cameraDataProvider,
		List<Ref<ISceneDataProvider>> sceneDataProviders)
	{
		// Acquire the render context and backbuffer that we'll be using
		Ref<RenderContext> renderContext;
		Ref<Image> backbuffer;
		if (!presenter->PrepareForRender(renderContext, backbuffer))
		{
			LogError(GetLogger(), "Failed to prepare presenter for rendering");
			return;
		}

		// Create our render view using the provider for camera data
		ManagedRef<RenderView> view = cameraDataProvider.GetRenderView(pipeline, presenter->GetBackbufferSize(), { backbuffer });

		// Add objects from the scene graph
		for(auto& sceneProvider : sceneDataProviders)
			sceneProvider->GetSceneData(view);

		// Actually render with the pipeline
		renderContext->Begin(view, pipeline);
		DoRender(*pipeline.Get(), *renderContext.Get());
		renderContext->End();

		// Submit the render data to the gpu and present
		if (!presenter->Present(renderContext))
		{
			LogError(GetLogger(), "Failed to present");
		}
	}

	void RenderingService::DoRender(RenderPipeline& pipeline, RenderContext& context) noexcept
	{
		try
		{
			context.RestoreViewport();

			// Do render!
			pipeline.Execute(context);
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

		_defaultDiffuseTexture = Engine::Get()->GetResourceLibrary()->CreateResource<Texture>(
			"RenderingService::DefaultDiffuseTexture", 
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

		_defaultCheckerTexture = Engine::Get()->GetResourceLibrary()->CreateResource<Texture>(
			"RenderingService::DefaultCheckerTexture",
			size, size,
			PixelFormat::RGBA8, ColorSpace::sRGB,
			ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled,
			ImageSamplerProperties(ImageFilterMode::Nearest, ImageSamplerProperties::Default.RepeatMode, ImageSamplerProperties::Default.MaxAnisotropy));

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

	void RenderingService::CreateDefaultNormalTexture()
	{
		LogInfo(GetLogger(), "Creating default normal texture...");

		constexpr int size = 32;
		constexpr int channels = 4;

		_defaultNormalTexture = Engine::Get()->GetResourceLibrary()->CreateResource<Texture>(
			"RenderingService::DefaultNormalTexture",
			size, size,
			PixelFormat::RGBA8, ColorSpace::Linear,
			ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled);

		List<uint8_t> pixelData(size * size * channels);

		for (uint64_t i = 0; i < pixelData.Count(); i += channels)
		{
			pixelData[i] = 127;
			pixelData[i + 1] = 127;
			pixelData[i + 2] = 255;
			pixelData[i + 3] = 255;
		}

		_defaultNormalTexture->SetPixels(0, pixelData.Count(), pixelData.Data());
	}
}