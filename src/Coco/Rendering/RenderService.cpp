#include "Renderpch.h"
#include "RenderService.h"
#include "Texture.h"
#include "Shader.h"
#include "Pipeline/RenderPipeline.h"
#include "Gizmos/GizmoRender.h"
#include "Graphics/RenderView.h"

//#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "Serializers/MaterialSerializer.h"
#include "Serializers/MeshSerializer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const string RenderService::sErrorShaderName = "error";

	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory, bool includeGizmoRendering) :
		_earlyTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleEarlyTick, sEarlyTickPriority)),
		_lateTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleLateTick, sLateTickPriority)),
		_gizmoRender(nullptr),
		_renderGizmos(false),
		_attachmentCache(),
		_currentFrameTasks(0),
		_maxFramesInFlight(1),
		_renderViewPool(),
		_platform(platformFactory.Create())
	{
		_device = _platform->CreateDevice(platformFactory.GetPlatformCreateParameters().DeviceCreateParameters);

		_contextPool = CreateUniqueRef<RenderContextPool>(*_device, 20);

		if (includeGizmoRendering)
		{
			_gizmoRender = CreateUniqueRef<GizmoRender>();
			_renderGizmos = true;
		}

		CreateDefaultDiffuseTexture();
		CreateDefaultNormalTexture();
		CreateDefaultCheckerTexture();
		CreateErrorShader();

		//MainLoop::Get()->AddListener(_earlyTickListener);
		MainLoop::Get()->AddListener(_lateTickListener);

		// Add resource serializers
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
		//resources.CreateSerializer<ShaderSerializer>();
		resources.CreateSerializer<TextureSerializer>();
		resources.CreateSerializer<MaterialSerializer>();
		resources.CreateSerializer<MeshSerializer>();

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		//MainLoop::Get()->RemoveListener(_earlyTickListener);
		MainLoop::Get()->RemoveListener(_lateTickListener);

		_defaultDiffuseTexture.reset();
		_defaultNormalTexture.reset();
		_defaultCheckerTexture.reset();
		_errorShader.reset();

		_contextPool.reset();
		_gizmoRender.reset();
		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}

	bool RenderService::Render(
		Ref<GraphicsPresenter> presenter,
		RenderPipeline& pipeline, 
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
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

		Ref<Image> backbuffer;

		if (!_currentFrameTasks.HasPresenter(presenter))
		{
			// Get the context used for rendering from the presenter
			Stopwatch syncStopwatch(true);
			Ref<RenderContext> context = _contextPool->Get(false);
			if (!presenter->PrepareForRender(context->GetRenderStartSemaphore(), backbuffer))
			{
				CocoError("Failed to prepare presenter for rendering")
				return false;
			}

			context->AddWaitOnSemaphore(context->GetRenderStartSemaphore());

			_currentFrameTasks.AddPresenter(presenter, context, syncStopwatch.Stop());
		}
		else
		{
			backbuffer = presenter->GetPreparedBackbuffer();
		}

		const CompiledRenderPipeline& compiledPipeline = pipeline.GetCompiledPipeline();
		std::array<Ref<Image>, 1> backbuffers{ backbuffer };
		SharedRef<RenderView> renderView = GetRenderView(presenter->GetID(), backbuffers, presenter->GetFramebufferSize(), compiledPipeline, renderViewProvider, sceneDataProviders);

		return _currentFrameTasks.QueueTask(presenter, renderView, compiledPipeline);
	}

	bool RenderService::Render(
		uint64 rendererID,
		std::span<Ref<Image>> framebuffers, 
		RenderPipeline& pipeline,
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
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

		const CompiledRenderPipeline& compiledPipeline = pipeline.GetCompiledPipeline();
		SharedRef<RenderView> renderView = GetRenderView(rendererID, framebuffers, framebufferSize, compiledPipeline, renderViewProvider, sceneDataProviders);

		return _currentFrameTasks.QueueTask(rendererID, renderView, compiledPipeline);
	}

	void RenderService::SetGizmoRendering(bool renderGizmos)
	{
		_renderGizmos = renderGizmos;
	}

	void RenderService::SetMaxFramesInFlight(uint32 framesInFlight)
	{
		_maxFramesInFlight = Math::Max(framesInFlight, static_cast<uint32>(1));
		OnMaxFramesInFlightChanged.Invoke(_maxFramesInFlight);
	}

	SharedRef<RenderView> RenderService::GetRenderView(
		uint64 rendererID,
		std::span<Ref<Image>> framebuffers, 
		const SizeInt& framebufferSize,
		const CompiledRenderPipeline& compiledPipeline,
		RenderViewProvider& renderViewProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		SharedRef<RenderView> renderView = _renderViewPool.Get();
		renderView->Reset();
		renderViewProvider.SetupRenderView(*renderView, compiledPipeline, rendererID, framebufferSize, framebuffers);

		// Get the scene data
		for (SceneDataProvider* provider : sceneDataProviders)
		{
			if (!provider)
				continue;

			provider->GatherSceneData(*renderView);
		}

		if (_gizmoRender && _renderGizmos)
			_gizmoRender->GatherSceneData(*renderView);

		return renderView;
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
				pixelData[baseIndex + 0] = (x % 2) ? 255 : 0;
				pixelData[baseIndex + 1] = (y % 2) ? 255 : 0;
				pixelData[baseIndex + 2] = ((x + y) % 2) ? 255 : 0;
				pixelData[baseIndex + 3] = 255;
				//pixelData[baseIndex + 0] = 255;
				//pixelData[baseIndex + 1] = 255;
				//pixelData[baseIndex + 2] = 255;
				//pixelData[baseIndex + 3] = 255;
				//
				//if (((x % 2) && (y % 2)) || !(y % 2))
				//{
				//	pixelData[baseIndex + 1] = 0; // Green = 0
				//	pixelData[baseIndex + 2] = 0; // Red = 0
				//}
			}
		}

		_defaultCheckerTexture->SetPixels(0, pixelData.data(), pixelData.size());

		CocoTrace("Created default checker texture");
	}

	void RenderService::CreateErrorShader()
	{
		_errorShader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			sErrorShaderName,
			std::vector<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Error.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Error.frag.glsl")
			}),
			GraphicsPipelineState(),
			std::vector<BlendState>({
				BlendState::Opaque
			}),
			VertexDataFormat(),
			GlobalShaderUniformLayout(),
			ShaderUniformLayout(),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
			)
		);
	}

	void RenderService::HandleEarlyTick(const TickInfo& tickInfo)
	{
		//Stopwatch waitForRenderSyncStopwatch(true);
		//
		//for (auto it = _renderTasks.begin(); it != _renderTasks.end(); it++)
		//{
		//	std::vector<RenderServiceRenderTask>& tasks = it->second;
		//
		//	if (tasks.size() == 0)
		//		continue;
		//
		//	RenderServiceRenderTask& lastTask = tasks.back();
		//	if (lastTask.Context.IsValid())
		//		lastTask.Context->WaitForRenderingToComplete();
		//}
		//
		//_stats.RenderSyncWait = waitForRenderSyncStopwatch.Stop();
	}

	void RenderService::HandleLateTick(const TickInfo& tickInfo)
	{
		_currentFrameTasks.Execute(*_contextPool);
		_lastFrameStats = _currentFrameTasks.GetStats();

		_device->ResetForNewFrame();
		_currentFrameTasks = FrameRenderTasks(tickInfo.TickNumber + 1);
	}
}