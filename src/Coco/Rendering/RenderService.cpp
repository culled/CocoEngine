#include "Renderpch.h"
#include "RenderService.h"
#include "Texture.h"
#include "Pipeline/RenderPipeline.h"
#include "Gizmos/GizmoRender.h"
#include "Graphics/RenderView.h"

#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "Serializers/MaterialSerializer.h"
#include "Serializers/MeshSerializer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderServiceRenderTask::RenderServiceRenderTask(Ref<RenderContext> context) :
		RenderServiceRenderTask(context, Ref<GraphicsPresenter>())
	{}

	RenderServiceRenderTask::RenderServiceRenderTask(Ref<RenderContext> context, Ref<GraphicsPresenter> presenter) :
		Context(std::move(context)),
		Presenter(presenter)
	{}

	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory, bool includeGizmoRendering) :
		_earlyTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleEarlyTick, sEarlyTickPriority)),
		_lateTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleLateTick, sLateTickPriority)),
		_renderTasks{},
		_gizmoRender(nullptr),
		_renderView(),
		_attachmentCache(),
		_renderContextCache()
	{
		_platform = platformFactory.Create();
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		if(includeGizmoRendering)
			_gizmoRender = CreateUniqueRef<GizmoRender>();

		CreateDefaultDiffuseTexture();
		CreateDefaultNormalTexture();
		CreateDefaultCheckerTexture();

		//MainLoop::Get()->AddListener(_earlyTickListener);
		MainLoop::Get()->AddListener(_lateTickListener);

		// Add resource serializers
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
		resources.CreateSerializer<ShaderSerializer>();
		resources.CreateSerializer<TextureSerializer>();
		resources.CreateSerializer<MaterialSerializer>();
		resources.CreateSerializer<MeshSerializer>();

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		//MainLoop::Get()->RemoveListener(_earlyTickListener);
		MainLoop::Get()->RemoveListener(_lateTickListener);
		_renderTasks.clear();

		_renderContextCache.clear();
		_individualRenderStats.clear();

		_defaultDiffuseTexture.reset();
		_defaultNormalTexture.reset();
		_defaultCheckerTexture.reset();

		_gizmoRender.reset();
		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}

	bool RenderService::Render(
		Ref<GraphicsPresenter> presenter,
		RenderPipeline& pipeline, 
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders,
		std::optional<RenderTask> dependsOn,
		RenderTask* outTask)
	{
		if (!presenter.IsValid())
		{
			CocoError("Presenter was invalid")
			return false;
		}

		if (pipeline.GetRenderPassCount() == 0)
			return false;

		if (!pipeline.Compile())
		{
			CocoError("RenderPipeline failed to compile")
			return false;
		}

		CompiledRenderPipeline compiledPipeline = pipeline.GetCompiledPipeline();

		// Get the context used for rendering from the presenter
		Ref<RenderContext> context;
		Ref<Image> backbuffer;
		Stopwatch presenterPrepareTime(true);
		if (!presenter->PrepareForRender(context, backbuffer))
		{
			CocoError("Failed to prepare presenter for rendering")
			return false;
		}

		_stats.RenderSyncWait += presenterPrepareTime.Stop();

		std::array<Ref<Image>, 1> backbuffers{ backbuffer };
		uint64 rendererID = presenter->GetID();

		bool success = Render(
			rendererID, 
			compiledPipeline, 
			backbuffers, 
			presenter->GetFramebufferSize(), 
			context, 
			renderViewProvider, 
			sceneDataProviders, 
			dependsOn, 
			outTask);
		_renderTasks[rendererID].emplace_back(context, presenter);
		return success;
	}

	bool RenderService::Render(
		uint64 rendererID,
		std::span<Ref<Image>> framebuffers, 
		RenderPipeline& pipeline,
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders, 
		std::optional<RenderTask> dependsOn,
		RenderTask* outTask)
	{
		if (framebuffers.size() == 0)
		{
			CocoError("No framebuffers were given. Please provide at least 1 framebuffer to render with")
			return false;
		}

		if (pipeline.GetRenderPassCount() == 0)
			return false;

		SizeInt framebufferSize;

		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			if (!framebuffers[i].IsValid())
			{
				CocoError("Framebuffer {} was invalid")
				return false;
			}

			const ImageDescription desc = framebuffers[i]->GetDescription();

			if (i == 0)
			{
				framebufferSize = SizeInt(desc.Width, desc.Height);
			}
			else if (desc.Width != framebufferSize.Width || desc.Height != framebufferSize.Height)
			{
				CocoError("Framebuffer {} did not have a size consistent with the first framebuffer. Expected: {}, actual: {}x{}", framebufferSize.ToString(), desc.Width, desc.Height)
				return false;
			}
		}

		if (!pipeline.Compile())
		{
			CocoError("RenderPipeline failed to compile")
			return false;
		}

		CompiledRenderPipeline compiledPipeline = pipeline.GetCompiledPipeline();
		Ref<RenderContext> context = GetReadyRenderContext();

		bool success = Render(rendererID, compiledPipeline, framebuffers, framebufferSize, context, renderViewProvider, sceneDataProviders, dependsOn, outTask);
		_renderTasks[rendererID].emplace_back(context);
		_orphanedRenderContexts.push_back(context);
		return success;
	}

	bool RenderService::Render(
		uint64 rendererID, 
		CompiledRenderPipeline& compiledPipeline,
		std::span<Ref<Image>> framebuffers,
		const SizeInt& framebufferSize,
		Ref<RenderContext> renderContext,
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders,
		std::optional<RenderTask> dependsOn,
		RenderTask* outTask)
	{
		// Create the RenderView using the given framebuffers
		_renderView.Reset();
		renderViewProvider.SetupRenderView(_renderView, compiledPipeline, rendererID, framebufferSize, framebuffers);

		// Get the scene data
		for (SceneDataProvider* provider : sceneDataProviders)
		{
			if (!provider)
				continue;

			provider->GatherSceneData(_renderView);
		}

		if (_gizmoRender)
			_gizmoRender->GatherSceneData(_renderView);

		Ref<GraphicsSemaphore> waitOn;

		// Add syncronization to the previous task if it was given
		if (dependsOn.has_value())
		{
			// Explicitly wait on the previous task's completion semaphore
			waitOn = dependsOn->RenderCompletedSemaphore;
		}
		else if (_renderTasks.contains(rendererID))
		{
			std::vector<RenderServiceRenderTask>& tasks = _renderTasks.at(rendererID);

			// If this renderer already performed a task this frame, wait on the last task's completion semaphore
			if (tasks.size() > 0)
			{
				RenderServiceRenderTask& task = tasks.back();
				Assert(task.Context.IsValid())

				waitOn = task.Context->GetRenderCompletedSemaphore();

				// Since the previous context is now being used, remove it from the orphaned list
				auto it = std::find(_orphanedRenderContexts.begin(), _orphanedRenderContexts.end(), task.Context);
				if (it != _orphanedRenderContexts.end())
					_orphanedRenderContexts.erase(it);
			}
		}
		else if(renderContext->GetWaitOnSemaphoreCount() == 0 && _orphanedRenderContexts.size() > 0)
		{
			// Use the last orphaned context as the signaller for this context
			renderContext->AddWaitOnSemaphore(_orphanedRenderContexts.back()->GetRenderCompletedSemaphore());
			_orphanedRenderContexts.pop_back();
		}

		if (!ExecuteRender(*renderContext, compiledPipeline, _renderView, waitOn))
		{
			_renderView.Reset();
			return false;
		}

		_stats += renderContext->GetRenderStats();
		_individualRenderStats.push_back(renderContext->GetRenderStats());
		_renderView.Reset();

		if (outTask)
			*outTask = RenderTask(rendererID, renderContext->GetRenderStats(), renderContext->GetRenderCompletedSemaphore(), renderContext->GetRenderCompletedFence());

		return true;
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

	Ref<RenderContext> RenderService::GetReadyRenderContext()
	{
		int earliestContextIndex = -1;
		double earliestTime = -1.0;

		for (int i = 0; i < _renderContextCache.size(); i++)
		{
			Ref<RenderContext>& context = _renderContextCache.at(i);

			if (context->CheckForRenderingComplete())
			{
				context->Reset();
				return context;
			}

			// Save this context if it started rendering the earliest
			double renderTime = context->GetLastRenderStartTime();
			if (earliestContextIndex == -1 || renderTime < earliestTime)
			{
				earliestContextIndex = i;
				earliestTime = renderTime;
			}
		}

		const int maxContexts = 20;
		if (_renderContextCache.size() == 0 || _renderContextCache.size() < maxContexts)
		{
			Ref<RenderContext>& context = _renderContextCache.emplace_back(_device->CreateRenderContext());

			CocoTrace("Created RenderContext for RenderService")

			return context;
		}
		else
		{
			Ref<RenderContext>& context = _renderContextCache.at(earliestContextIndex);
			context->WaitForRenderingToComplete();
			context->Reset();
			return context;
		}
	}
}