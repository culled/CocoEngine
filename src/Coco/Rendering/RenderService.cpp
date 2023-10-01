#include "Renderpch.h"
#include "RenderService.h"
#include "Texture.h"
#include "Pipeline/RenderPipeline.h"
#include "RenderDebug/DebugRender.h"

#include "Serializers/ShaderSerializer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderServiceRenderTask::RenderServiceRenderTask(Ref<RenderContext> context, Ref<GraphicsPresenter> presenter) :
		Context(std::move(context)),
		Presenter(presenter)
	{}

	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory, bool includeDebugRendering) :
		_earlyTickListener(CreateUniqueRef<TickListener>(this, &RenderService::HandleEarlyTick, sEarlyTickPriority)),
		_lateTickListener(CreateUniqueRef<TickListener>(this, &RenderService::HandleLateTick, sLateTickPriority)),
		_renderTasks{},
		_debugRender(nullptr),
		_renderView()
	{
		_platform = platformFactory.Create();
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		if(includeDebugRendering)
			_debugRender = CreateUniqueRef<DebugRender>();

		CreateDefaultDiffuseTexture();
		CreateDefaultNormalTexture();
		CreateDefaultCheckerTexture();

		//MainLoop::Get()->AddListener(*_earlyTickListener);
		MainLoop::Get()->AddListener(*_lateTickListener);

		// Add resource serializers
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
		resources.CreateSerializer<ShaderSerializer>();

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		//MainLoop::Get()->RemoveListener(*_earlyTickListener);
		MainLoop::Get()->RemoveListener(*_lateTickListener);
		_renderTasks.clear();

		_defaultDiffuseTexture.Invalidate();
		_defaultNormalTexture.Invalidate();
		_defaultCheckerTexture.Invalidate();

		_debugRender.reset();
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
		Stopwatch presenterPrepareTime(true);
		if (!presenter->PrepareForRender(context, backbuffer))
		{
			CocoError("Failed to prepare presenter for rendering")
			return RenderTask();
		}

		_stats.RenderSyncWait += presenterPrepareTime.Stop();

		// Create the RenderView using the acquired backbuffers
		std::array<Ref<Image>, 1> backbuffers{ backbuffer };
		_renderView.Reset();
		renderViewProvider.SetupRenderView(_renderView, compiledPipeline, presenter->GetID(), presenter->GetFramebufferSize(), backbuffers);

		// Get the scene data
		for (SceneDataProvider* provider : sceneDataProviders)
		{
			if (!provider)
				continue;

			provider->GatherSceneData(_renderView);
		}

		if (_debugRender)
			_debugRender->GatherSceneData(_renderView);

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

		if (!ExecuteRender(*context, compiledPipeline, _renderView, waitOn))
		{
			// The render failed, so don't save it as a task
			return RenderTask();
		}

		_stats += context->GetRenderStats();
		_individualRenderStats.push_back(context->GetRenderStats());

		RenderTask task(context->GetRenderCompletedSemaphore(), context->GetRenderCompletedFence());
		_renderTasks[presenterID].emplace_back(context, presenter);
		return task;
	}

	bool RenderService::ExecuteRender(
		RenderContext& context, 
		CompiledRenderPipeline& compiledPipeline, 
		RenderView& renderView,
		Ref<GraphicsSemaphore> waitOn)
	{
		context.SetCurrentRenderView(renderView);

		try
		{
			// Go through each pass and prepare it
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				it->Pass->Prepare(context, renderView);
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error preparing render: {}", ex.what())

			return false;
		}

		try
		{
			if (waitOn.IsValid())
			{
				context.AddWaitOnSemaphore(waitOn);
			}

			// Go through each pass and execute it
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				if (it == compiledPipeline.RenderPasses.begin())
				{
					if (!context.Begin(compiledPipeline))
						return false;
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
			CocoError("Error executing render: {}", ex.what())
		}

		context.End();

		return true;
	}

	void RenderService::CreateDefaultDiffuseTexture()
	{
		constexpr uint32 size = 32;
		constexpr uint8 channels = 4;

		_defaultDiffuseTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			"Default Diffuse Texture",
			ImageDescription(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
			ImageSamplerDescription::LinearRepeat
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint64 i = 0; i < pixelData.size(); i++)
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
			ImageSamplerDescription::LinearRepeat
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint64 i = 0; i < pixelData.size(); i += channels)
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
			ImageSamplerDescription::NearestRepeat
		);

		std::vector<uint8> pixelData(size * size * channels);

		for (uint64 x = 0; x < size; x++)
		{
			for (uint64 y = 0; y < size; y++)
			{
				const uint64 baseIndex = ((x * size) + y) * channels;
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

	void RenderService::HandleEarlyTick(const TickInfo& tickInfo)
	{
		Stopwatch waitForRenderSyncStopwatch(true);

		for (auto it = _renderTasks.begin(); it != _renderTasks.end(); it++)
		{
			std::vector<RenderServiceRenderTask>& tasks = it->second;

			if (tasks.size() == 0)
				continue;

			RenderServiceRenderTask& lastTask = tasks.back();
			if (lastTask.Context.IsValid())
				lastTask.Context->WaitForRenderingToComplete();
		}

		_stats.RenderSyncWait = waitForRenderSyncStopwatch.Stop();
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

		_individualRenderStats.clear();
		_stats.Reset();
		_renderTasks.clear();

		_device->ResetForNewFrame();
	}
}