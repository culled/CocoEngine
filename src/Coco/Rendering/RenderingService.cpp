#include "RenderingService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Types/Array.h>
#include "Graphics/GraphicsDevice.h"
#include "Loaders/TextureLoader.h"
#include "Loaders/ShaderLoader.h"
#include "Loaders/MaterialLoader.h"
#include "Texture.h"
#include "Components/MeshRendererComponent.h"
#include <Coco/Core/Scene/Components/TransformComponent.h>

namespace Coco::Rendering
{
	RenderingService* RenderingService::s_instance = nullptr;

	RenderingService::RenderingService(EngineServiceManager* serviceManager, const GraphicsPlatformCreationParameters& backendCreateParams) : EngineService(serviceManager)
	{
		s_instance = this;
		_graphics = GraphicsPlatform::CreatePlatform(this, backendCreateParams);
		
		// Create default textures
		CreateDefaultDiffuseTexture();
		CreateDefaultCheckerTexture();

		RegisterTickListener(this, &RenderingService::PurgeTick, ResourcePurgeTickPriority);

		ResourceLibrary* resourceLibrary = ServiceManager->Engine->GetResourceLibrary();
		resourceLibrary->CreateLoader<TextureLoader>();
		resourceLibrary->CreateLoader<ShaderLoader>();
		resourceLibrary->CreateLoader<MaterialLoader>();
	}

	RenderingService::~RenderingService()
	{
		_defaultPipeline.reset();
		_defaultDiffuseTexture.reset();
		_defaultCheckerTexture.reset();
		_graphics.reset();
	}

	void RenderingService::Render(const WeakManagedRef<GraphicsPresenter>& presenter)
	{
		if (_defaultPipeline)
		{
			Ref<Scene> scene = Engine::Get()->GetApplication()->GetScene();
			List<Ref<SceneEntity>> cameras = scene->GetEntitiesWithComponent<CameraComponent>();

			if (cameras.Count() == 0)
			{
				LogError(GetLogger(), "Failed to render presenter: No camera to render from");
				return;
			}

			Render(presenter, _defaultPipeline, cameras.First()->GetComponent<CameraComponent>());
		}
		else
		{
			LogError(GetLogger(), "Failed to render presenter: No render pipeline was given and no default render pipeline has been set");
		}
	}

	void RenderingService::Render(const WeakManagedRef<GraphicsPresenter>& presenter, const Ref<RenderPipeline>& pipeline, CameraComponent* camera)
	{
		SizeInt size = presenter->GetBackbufferSize();

		// Make sure the camera matches our rendering aspect ratio
		camera->SetAspectRatio(static_cast<double>(size.Width) / size.Height);

		// Acquire the render context and backbuffer that we'll be using
		RenderContext* renderContext;
		WeakManagedRef<Image> backbuffer;
		if (!presenter->PrepareForRender(renderContext, backbuffer))
		{
			LogError(GetLogger(), "Failed to prepare presenter for rendering");
			return;
		}

		camera->SetRenderTargetOverrides({ backbuffer });

		Ref<RenderView> view = CreateRef<RenderView>(
			RectInt(Vector2Int::Zero, size), 
			pipeline->GetClearColor(), 
			camera->GetProjectionMatrix(), 
			camera->GetViewMatrix(),
			camera->GetRenderTargets(pipeline, size));

		// Add objects from the scene graph
		Ref<Scene> scene = Engine::Get()->GetApplication()->GetScene();
		List<Ref<SceneEntity>> renderEntities = scene->GetEntitiesWithComponent<MeshRendererComponent>();

		for (const Ref<SceneEntity>& entity : renderEntities)
		{
			MeshRendererComponent* renderComp = entity->GetComponent<MeshRendererComponent>();
			TransformComponent* transformComp = entity->GetComponent<TransformComponent>();
			view->AddRenderObject(renderComp->GetMesh(), renderComp->GetMaterial(), transformComp->GetTransformMatrix());
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

	void RenderingService::PurgeTick(double deltaTime)
	{
		_timeSinceLastPurge += deltaTime;

		if (_timeSinceLastPurge > PurgeFrequency)
		{
			if(_graphics != nullptr && _graphics->GetDevice() != nullptr)
				_graphics->GetDevice()->PurgeUnusedResources();

			_timeSinceLastPurge = 0.0;
		}
	}
}