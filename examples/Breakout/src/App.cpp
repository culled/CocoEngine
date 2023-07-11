#include "App.h"

#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Engine.h>

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/CameraComponent.h>
#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include "Units/Player.h"

#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "Rendering/OpaqueRenderPass.h"
#include <Coco/Rendering/MeshPrimitives.h>

MainApplication(App)

App* App::_instance = nullptr;

App::App() : Application("Breakout"), 
	_playerEntity(InvalidEntityID), _cameraEntity(InvalidEntityID)
{
	_instance = this;

	// Setup logging
	SharedRef<Logging::ConsoleLogSink> consoleSink = CreateSharedRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	_logger->AddSink(consoleSink);
	Engine::Get()->GetLogger()->AddSink(consoleSink);

	// Setup our services
	EngineServiceManager* serviceManager = Engine::Get()->GetServiceManager();
	_ecsService = serviceManager->CreateService<ECS::ECSService>();
	_inputService = serviceManager->CreateService<Input::InputService>();
	_renderingService = serviceManager->CreateService<Rendering::RenderingService>(Rendering::GraphicsPlatformCreationParameters(Name, Rendering::RenderingRHI::Vulkan));
	_windowingService = serviceManager->CreateService<Windowing::WindowingService>();

	// Add our render tick
	Engine::Get()->GetMainLoop()->CreateTickListener(this, &App::RenderTick, 1000);

	ConfigureRenderPipeline();

	LogTrace(_logger, "App setup complete");
}

App::~App()
{
	LogTrace(_logger, "App shutdown");
}

void App::Start()
{
	using namespace Coco::Windowing;

	auto window = _windowingService->CreateNewWindow(WindowCreateParameters(Name, SizeInt(1280, 720)));
	window->Show();

	CreateCamera();
	CreatePlayer();
	CreateArena();
}

void App::ConfigureRenderPipeline()
{
	using namespace Coco::Rendering;
	ResourceLibrary* library = Engine::Get()->GetResourceLibrary();

	Ref<RenderPipeline> renderPipeline = library->CreateResource<RenderPipeline>("RenderPipeline");

	// Color attachment will be index 0, depth at index 1
	renderPipeline->AddRenderPass(CreateSharedRef<OpaqueRenderPass>(), { 0, 1 });
	renderPipeline->SetClearColor(_clearColor);

	_renderingService->SetDefaultPipeline(renderPipeline);

	_basicShader = library->CreateResource<Shader>("Basic Shader");

	auto pipelineState = GraphicsPipelineState();
	pipelineState.CullingMode = CullMode::None;

	_basicShader->CreateSubshader(
		"main",
		{
			{ ShaderStageType::Vertex, "shaders/built-in/ObjectShader.vert.spv" },
			{ ShaderStageType::Fragment, "shaders/built-in/ObjectShader.frag.spv" },
		},
		pipelineState,
		{
			ShaderVertexAttribute(BufferDataFormat::Vector3),
			ShaderVertexAttribute(BufferDataFormat::Vector2)
		},
		{
			ShaderDescriptor("_BaseColor", BufferDataFormat::Vector4)
		},
		{
			ShaderTextureSampler("_MainTex")
		}
		);

	_wallMaterial = library->CreateResource<Material>("Material::Wall", _basicShader);
	_wallMaterial->SetVector4("_BaseColor", Color::Green);
	_wallMaterial->SetTexture("_MainTex", App::Get()->GetRenderingService()->GetDefaultDiffuseTexture());
}

void App::CreateCamera()
{
	using namespace Coco::ECS;

	_cameraEntity = _ecsService->CreateEntity("Camera");

	auto& cameraTransform = _ecsService->AddComponent<TransformComponent>(_cameraEntity);
	cameraTransform.SetGlobalPosition(_cameraPosition);
	cameraTransform.SetGlobalEulerAngles(_cameraRotation);

	auto& camera = _ecsService->AddComponent<CameraComponent>(_cameraEntity);
	camera.SetOrthographicProjection(_cameraOrthoSize, 1.0, 1.0, 30.0);
}

void App::CreatePlayer()
{
	_playerEntity = _ecsService->CreateEntity("Player");
	_ecsService->AddComponent<Player>(_playerEntity);
}

void App::CreateArena()
{
	Ref<Mesh> wallMesh = MeshPrimitives::CreateXYPlane("Wall Mesh", Size(1.0, _arenaSize.Height));
	wallMesh->UploadData();

	Ref<Mesh> ceilingMesh = MeshPrimitives::CreateXYPlane("Ceiling Mesh", Size(_arenaSize.Width, 1.0));
	ceilingMesh->UploadData();

	EntityID leftWall = _ecsService->CreateEntity("Left Wall");
	auto& leftWallTransform = _ecsService->AddComponent<TransformComponent>(leftWall);
	leftWallTransform.SetGlobalPosition(_arenaOffset + Vector3::Left * _arenaSize.Width * 0.5);
	_ecsService->AddComponent<MeshRendererComponent>(leftWall, wallMesh, _wallMaterial);

	EntityID rightWall = _ecsService->CreateEntity("Right Wall");
	auto& rightWallTransform = _ecsService->AddComponent<TransformComponent>(rightWall);
	rightWallTransform.SetGlobalPosition(_arenaOffset + Vector3::Right * _arenaSize.Width * 0.5);
	_ecsService->AddComponent<MeshRendererComponent>(rightWall, wallMesh, _wallMaterial);

	EntityID ceiling = _ecsService->CreateEntity("Ceiling");
	auto& ceilingTransform = _ecsService->AddComponent<TransformComponent>(ceiling);
	ceilingTransform.SetGlobalPosition(_arenaOffset + Vector3::Forwards * (_arenaSize.Height * 0.5 - 0.5));
	_ecsService->AddComponent<MeshRendererComponent>(ceiling, ceilingMesh, _wallMaterial);
}

void App::RenderTick(double deltaTime)
{
	_renderingService->Render(_windowingService->GetMainWindow()->GetPresenter(), _ecsService->GetComponent<CameraComponent>(_cameraEntity), *_ecsService->GetRootScene());
}
