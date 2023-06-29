#include "App.h"

#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Engine.h>

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/CameraComponent.h>
#include <Coco/ECS/Components/TransformComponent.h>
#include "Units/Player.h"

#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "Rendering/OpaqueRenderPass.h"

MainApplication(App)

App* App::_instance = nullptr;

App::App(Coco::Engine* engine) : Application(engine, "Breakout"), 
	_playerEntity(InvalidEntityID), _cameraEntity(InvalidEntityID)
{
	_instance = this;

	// Setup logging
	SharedRef<Logging::ConsoleLogSink> consoleSink = CreateSharedRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	// Setup our services
	EngineServiceManager* serviceManager = engine->GetServiceManager();
	_ecsService = serviceManager->CreateService<ECS::ECSService>();

	_inputService = serviceManager->CreateService<Input::InputService>();
	_renderingService = serviceManager->CreateService<Rendering::RenderingService>(Rendering::GraphicsPlatformCreationParameters(Name, Rendering::RenderingRHI::Vulkan));
	_windowingService = serviceManager->CreateService<Windowing::WindowingService>();

	// Add our render tick
	this->Engine->GetMainLoop()->CreateTickListener(this, &App::RenderTick, 1000);

	ConfigureRenderPipeline();

	LogTrace(Logger, "App setup complete");
}

App::~App()
{
	LogTrace(Logger, "App shutdown");
}

void App::Start()
{
	using namespace Coco::Windowing;

	auto window = _windowingService->CreateNewWindow(WindowCreateParameters(Name, SizeInt(1280, 720)));
	window->Show();

	CreateCamera();
	CreatePlayer();
}

void App::ConfigureRenderPipeline()
{
	using namespace Coco::Rendering;
	Ref<RenderPipeline> renderPipeline = this->Engine->GetResourceLibrary()->CreateResource<RenderPipeline>("RenderPipeline", ResourceLibrary::DefaultTickLifetime);

	// Color attachment will be index 0, depth at index 1
	renderPipeline->AddRenderPass(CreateSharedRef<OpaqueRenderPass>(), { 0, 1 });
	renderPipeline->SetClearColor(_clearColor);

	_renderingService->SetDefaultPipeline(renderPipeline);
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
	_ecsService->AddComponent<ScriptComponent>(_playerEntity, CreateManagedRef<Player>());
}

void App::RenderTick(double deltaTime)
{
	_renderingService->Render(_windowingService->GetMainWindow()->GetPresenter(), &_ecsService->GetComponent<CameraComponent>(_cameraEntity), _ecsService->GetRootScene());
}
