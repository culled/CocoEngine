#include "Renderpch.h"
#include "RenderService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderServiceRenderTask::RenderServiceRenderTask(Ref<RenderContext> context, Ref<GraphicsPresenter> presenter) :
		Context(std::move(context)),
		Presenter(presenter)
	{}

	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory) :
		_lateTickListener(CreateUniqueRef<TickListener>(this, &RenderService::HandleLateTick, sLateTickPriority)),
		_renderTasks{}
	{
		_platform = platformFactory.Create();
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		CreateDefaultDiffuseTexture();
		CreateDefaultNormalTexture();
		CreateDefaultCheckerTexture();

		MainLoop::Get()->AddListener(*_lateTickListener);

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		MainLoop::Get()->RemoveListener(*_lateTickListener);
		_renderTasks.clear();

		_defaultDiffuseTexture.Invalidate();
		_defaultNormalTexture.Invalidate();
		_defaultCheckerTexture.Invalidate();

		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}

	RenderTask RenderService::Render(
		Ref<GraphicsPresenter> presenter,
		RenderPipeline& pipeline, 
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders,
		std::optional<RenderTask> dependsOn)
	{
		if (!presenter.IsValid())
		{
			CocoError("Presenter was invalid")
			return RenderTask();
		}

		if (!pipeline.Compile())
			return RenderTask();

		CompiledRenderPipeline compiledPipeline = pipeline.GetCompiledPipeline();

		// Get the context used for rendering from the presenter
		Ref<RenderContext> context;
		Ref<Image> backbuffer;
		if (!presenter->PrepareForRender(context, backbuffer))
		{
			CocoError("Failed to prepare presenter for rendering")
			return RenderTask();
		}

		// Create the RenderView using the acquired backbuffers
		std::array<Ref<Image>, 1> backbuffers{ backbuffer };
		UniqueRef<RenderView> view = renderViewProvider.CreateRenderView(compiledPipeline, presenter->GetID(), presenter->GetFramebufferSize(), backbuffers);

		// Get the scene data
		for (SceneDataProvider* provider : sceneDataProviders)
		{
			if (!provider)
				continue;

			provider->GatherSceneData(*view);
		}

		Ref<GraphicsSemaphore> waitOn;
		uint64 presenterID = presenter->GetID();

		// Add syncronization to the previous task if it was given
		if (dependsOn.has_value())
		{
			waitOn = dependsOn->RenderCompletedSemaphore;
		}
		else if(_renderTasks.contains(presenterID))
		{
			std::vector<RenderServiceRenderTask>& tasks = _renderTasks.at(presenterID);

			if (tasks.size() > 0)
			{
				RenderServiceRenderTask& task = tasks.back();
				Assert(task.Context.IsValid())
				waitOn = task.Context->GetRenderCompletedSemaphore();
			}
		}

		ExecuteRender(*context, compiledPipeline, *view, waitOn);

		RenderTask task(context->GetRenderCompletedSemaphore(), context->GetRenderCompletedFence());
		_renderTasks[presenterID].emplace_back(context, presenter);
		return task;
	}

	void RenderService::ExecuteRender(RenderContext& context, CompiledRenderPipeline& compiledPipeline, RenderView& renderView, Ref<GraphicsSemaphore> waitOn)
	{
		const EnginePlatform& platform = Engine::cGet()->GetPlatform();
		double pipelineStartTime = platform.GetSeconds();
		std::unordered_map<string, TimeSpan> passExecutionTimes;

		try
		{
			// Go through each pass and execute it
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				double passStartTime = platform.GetSeconds();

				if (it == compiledPipeline.RenderPasses.begin())
				{
					if (!context.Begin(renderView, compiledPipeline))
						return;

					if (waitOn.IsValid())
					{
						context.AddWaitOnSemaphore(waitOn);
					}
				}
				else
				{
					if (!context.BeginNextPass())
						break;
				}

				it->Pass->Execute(context, renderView);

				_stats.PassExecutionTime[it->Pass->GetName()] += TimeSpan::FromSeconds(platform.GetSeconds() - passStartTime);
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error during render: {}", ex.what())
		}

		context.End();

		_stats.PipelineExecutionTime += TimeSpan::FromSeconds(platform.GetSeconds() - pipelineStartTime);
		_stats += *context.GetRenderStats();
	}

	void RenderService::CreateDefaultDiffuseTexture()
	{
		constexpr uint32 size = 32;
		constexpr uint8 channels = 4;

		_defaultDiffuseTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			"Default Diffuse Texture",
			ImageDescription(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
			ImageSamplerDescription()
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint64_t i = 0; i < pixelData.size(); i++)
		{
			pixelData[i] = 255;
		}

		_defaultDiffuseTexture->SetPixels(0, pixelData.data(), pixelData.size());

		CocoTrace("Created default diffuse texture");
	}

	void RenderService::CreateDefaultNormalTexture()
	{
		constexpr uint32 size = 32;
		constexpr uint8 channels = 4;

		_defaultNormalTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			"Default Normal Texture",
			ImageDescription(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::Linear, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
			ImageSamplerDescription()
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint64_t i = 0; i < pixelData.size(); i += channels)
		{
			pixelData[i] = 127;
			pixelData[i + 1] = 127;
			pixelData[i + 2] = 255;
			pixelData[i + 3] = 255;
		}

		_defaultNormalTexture->SetPixels(0, pixelData.data(), pixelData.size());

		CocoTrace("Created default normal texture");
	}

	void RenderService::CreateDefaultCheckerTexture()
	{
		constexpr uint32 size = 32;
		constexpr uint8 channels = 4;

		_defaultCheckerTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			"Default Checker Texture",
			ImageDescription(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
			ImageSamplerDescription(ImageFilterMode::Nearest, ImageRepeatMode::Repeat, MipMapFilterMode::Nearest, 1)
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint32 x = 0; x < size; x++)
		{
			for (uint32 y = 0; y < size; y++)
			{
				const uint32 baseIndex = ((x * size) + y) * channels;
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

		_defaultCheckerTexture->SetPixels(0, pixelData.data(), pixelData.size());

		CocoTrace("Created default checker texture");
	}

	void RenderService::HandleLateTick(const TickInfo& tickInfo)
	{
		// Queue presentation for each rendered presenter
		for (auto it = _renderTasks.begin(); it != _renderTasks.end(); it++)
		{
			std::vector<RenderServiceRenderTask>& tasks = it->second;

			if (tasks.size() == 0)
				continue;

			RenderServiceRenderTask& lastTask = tasks.back();
			Assert(lastTask.Context.IsValid())

			if (lastTask.Presenter.IsValid())
			{
				lastTask.Presenter->Present(lastTask.Context->GetRenderCompletedSemaphore());
			}
		}

		// Wait for all renders to complete
		for (auto it = _renderTasks.begin(); it != _renderTasks.end(); it++)
		{
			std::vector<RenderServiceRenderTask>& tasks = it->second;

			if (tasks.size() == 0)
				continue;

			RenderServiceRenderTask& lastTask = tasks.back();
			Assert(lastTask.Context.IsValid())

			lastTask.Context->WaitForRenderingToComplete();
		}

		_renderTasks.clear();

		_device->ResetForNewFrame();
		_stats.Reset();
	}
}