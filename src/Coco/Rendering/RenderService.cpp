#include "Renderpch.h"
#include "RenderService.h"
#include "Pipeline/RenderPipeline.h"
#include "Graphics/RenderFrame.h"
#include "Graphics/Image.h"
#include "Texture.h"
#include "Shader.h"

#include <Coco/Core/Resources/ResourceSerializerFactory.h>
#include "Serializers/MaterialSerializer.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "Serializers/MeshSerializer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const string RenderService::ErrorShaderName = "error";

	RenderService::RenderService(const GraphicsPlatformFactory& platformFactory) :
		_attachmentCache(nullptr),
		_earlyTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleEarlyTick, EarlyTickPriority)),
		_lateTickListener(CreateManagedRef<TickListener>(this, &RenderService::HandleLateTick, LateTickPriority)),
		_framesInFlight(),
		_maxFramesInFlight(1),
		_platform(platformFactory.Create())
	{
		_device = _platform->CreateDevice();

		CreateDefaultDiffuseTexture();
		CreateDefaultNormalTexture();
		CreateDefaultCheckerTexture();
		CreateErrorShader();

		_attachmentCache = CreateUniqueRef<AttachmentCache>();

		MainLoop::Get()->AddTickListener(_earlyTickListener);
		MainLoop::Get()->AddTickListener(_lateTickListener);

		// Add resource serializers
		// TODO: this will be replaced with the auto-registering serializers
		RegisterResourceSerializers();

		CocoTrace("RenderService initialized")
	}

	RenderService::~RenderService()
	{
		MainLoop::Get()->RemoveTickListener(_earlyTickListener);
		MainLoop::Get()->RemoveTickListener(_lateTickListener);
		_framesInFlight.clear();

		_defaultDiffuseTexture.reset();
		_defaultNormalTexture.reset();
		_defaultCheckerTexture.reset();
		_errorShader.reset();

		//_gizmoRender.reset();
		_device.reset();
		_platform.reset();

		CocoTrace("RenderService shutdown")
	}

	bool RenderService::Render(
		uint64 rendererID,
		Ref<Presenter> presenter, 
		RenderPipeline& pipeline, 
		RenderViewDataProvider& renderViewDataProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		const CompiledRenderPipeline* compiledPipeline = nullptr;
		if(!TryCompilePipeline(pipeline, compiledPipeline))
			return false;

		SharedRef<RenderFrame> currentFrame = EnsureRenderFrame();
		return currentFrame->Render(rendererID, presenter, *compiledPipeline, renderViewDataProvider, sceneDataProviders);
	}

	bool RenderService::Render(
		uint64 rendererID, 
		std::span<Ref<Image>> framebuffers, 
		RenderPipeline& pipeline, 
		RenderViewDataProvider& renderViewDataProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		const CompiledRenderPipeline* compiledPipeline = nullptr;
		if (!TryCompilePipeline(pipeline, compiledPipeline))
			return false;

		SizeInt framebufferSize;
		
		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			if (!framebuffers[i].IsValid())
			{
				CocoError("Framebuffer {} was invalid")
				return false;
			}
		
			const ImageDescription& desc = framebuffers[i]->GetDescription();
		
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

		SharedRef<RenderFrame> currentFrame = EnsureRenderFrame();
		return currentFrame->Render(rendererID, framebuffers, framebufferSize, *compiledPipeline, renderViewDataProvider, sceneDataProviders);
	}

	void RenderService::SetMaxFramesInFlight(uint32 framesInFlight)
	{
		_maxFramesInFlight = Math::Max(framesInFlight, static_cast<uint32>(1));
		OnMaxFramesInFlightChanged.Invoke(_maxFramesInFlight);
	}

	void RenderService::RegisterResourceSerializers()
	{
		ResourceSerializerFactory::Register<MaterialSerializer>("Material");
		ResourceSerializerFactory::Register<ShaderSerializer>("Shader");
		ResourceSerializerFactory::Register<TextureSerializer>("Texture");
		ResourceSerializerFactory::Register<MeshSerializer>("Mesh");
	}

	void RenderService::CreateDefaultDiffuseTexture()
	{
		constexpr uint32 size = 32;
		constexpr uint8 channels = 4;

		_defaultDiffuseTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			ResourceID("RenderService::DefaultDiffuseTexture"),
			ImageDescription::Create2D(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
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
			ResourceID("RenderService::DefaultNormalTexture"),
			ImageDescription::Create2D(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::Linear, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
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
			ResourceID("RenderService::DefaultCheckerTexture"),
			ImageDescription::Create2D(size, size, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled, false),
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
			}
		}

		_defaultCheckerTexture->SetPixels(0, pixelData.data(), pixelData.size());

		CocoTrace("Created default checker texture");
	}

	bool RenderService::TryCompilePipeline(RenderPipeline& pipeline, const CompiledRenderPipeline*& outCompiledPipeline)
	{
		if (pipeline.GetRenderPassBindings().size() == 0)
			return false;

		try
		{
			outCompiledPipeline = &pipeline.GetOrCompile();
			return true;
		}
		catch (const RenderPipelineCompileException& ex)
		{
			CocoError("RenderPipeline \"{}\" failed to compile: {}", pipeline.GetID().ToString(), ex.what())
			return false;
		}
	}

	void RenderService::CreateErrorShader()
	{
		_errorShader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			ResourceID("RenderService::ErrorShader"),
			ErrorShaderName,
			std::initializer_list<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Error.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Error.frag.glsl")
				}),
			GraphicsPipelineState(),
			std::initializer_list<AttachmentBlendState>({
				AttachmentBlendState::Opaque
				}),
			VertexDataFormat(),
			ShaderUniformLayout::Empty,
			ShaderUniformLayout::Empty,
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderUniformType::Matrix4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
			)
		);
	}

	void RenderService::HandleEarlyTick(const TickInfo& tickInfo)
	{
		_attachmentCache->PurgeUnusedAttachments();
	}

	void RenderService::HandleLateTick(const TickInfo& tickInfo)
	{
 		_device->EndRenderFrame();
	}

	SharedRef<RenderFrame> RenderService::EnsureRenderFrame()
	{
		SharedRef<RenderFrame> current = _device->GetCurrentRenderFrame();
		if (current)
			return current;

		if (_framesInFlight.size() >= _maxFramesInFlight)
		{
			auto it = _framesInFlight.begin();
			SharedRef<RenderFrame> renderFrame = *it;
			renderFrame->WaitForRenderToComplete();
			_framesInFlight.erase(it);
		}

		return _framesInFlight.emplace_back(_device->StartNewRenderFrame());
	}
}