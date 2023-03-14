#include "CocoSandboxApplication.h"

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Logging/Sinks/FileLogSink.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/RenderingService.h>

#include "HelloTriangleRenderPass.h"

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication(Coco::Engine* engine) : 
	Coco::Application(engine, "Coco Sandbox"),
	_tickListener(new Coco::MainLoopTickListener(this, &CocoSandboxApplication::Tick, 0))
{
	Ref<Logging::ConsoleLogSink> consoleSink = CreateRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	Ref<Logging::FileLogSink> fileSink = CreateRef<Logging::FileLogSink>(Logging::LogLevel::Trace, "coco.log");
	Logger->AddSink(fileSink);
	engine->GetLogger()->AddSink(fileSink);

	_inputService = engine->GetServiceManager()->CreateService<Input::InputService>();

	Rendering::GraphicsPlatformCreationParameters createParams(Name, Rendering::RenderingRHI::Vulkan);
	_renderService = engine->GetServiceManager()->CreateService<Rendering::RenderingService>(createParams);

	Ref<Rendering::RenderPipeline> pipeline = CreateRef<Rendering::RenderPipeline>();
	pipeline->SetClearColor(Color(0.1, 0.2, 0.3));

	List<int> attachmentMapping;
	attachmentMapping.Add(0);

	pipeline->AddRenderPass(CreateRef<HelloTriangleRenderPass>(), attachmentMapping);
	_renderService->SetDefaultPipeline(pipeline);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	_camera = CreateRef<CameraComponent>();
	_camera->SetPerspectiveProjection(90.0, 1.0, 0.1, 100.0);
	//_camera->SetOrthographicProjection(10.0, 1.0, 0.1, 100.0);
	_cameraPosition = Vector3(0.0, 0.0, 0.0);

	//engine->GetMainLoop()->SetTargetTickRate(2);

	LogInfo(Logger, "Sandbox application created");
}

CocoSandboxApplication::~CocoSandboxApplication()
{
	this->Engine->GetMainLoop()->RemoveTickListener(_tickListener);
	_tickListener.reset();

	LogInfo(Logger, "Sandbox application destroyed");
}

void CocoSandboxApplication::Start()
{
	Windowing::WindowCreateParameters windowCreateParams("Coco Sandbox", SizeInt(1280, 720));
	_window = _windowService->CreateNewWindow(windowCreateParams);
	_window->Show();

	_inputService->GetKeyboard()->OnKeyPressedEvent += [&](Input::KeyboardKey key) {
		//LogInfo(Logger, FormattedString("Pressed key {}", static_cast<int>(key)));
		if (key == Input::KeyboardKey::Escape)
		{
			Quit();
			return true;
		}

		return false;
		};

	this->Engine->GetMainLoop()->AddTickListener(_tickListener);

	LogInfo(Logger, "Sandbox application started");
}

void CocoSandboxApplication::Tick(double deltaTime)
{
	Vector2 mouseDelta = _inputService->GetMouse()->GetDelta();

	_cameraEulerAngles.Z -= mouseDelta.X * 0.005;
	_cameraEulerAngles.X = Math::Clamp(_cameraEulerAngles.X - mouseDelta.Y * 0.005, Math::Deg2Rad(-90.0), Math::Deg2Rad(90.0));

	//if (mouseDelta.GetLength() > 0.5)
	//	LogInfo(Logger, FormattedString("Tilt: {}, Yaw: {}", Math::Rad2Deg(_cameraEulerAngles.X), Math::Rad2Deg(_cameraEulerAngles.Z)));

	Quaternion orientation = Quaternion(_cameraEulerAngles);

	Input::Keyboard* keyboard = _inputService->GetKeyboard();
	Vector3 velocity;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::W))
		velocity += orientation * Vector3::Forwards * 5.0;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::S))
		velocity += orientation * Vector3::Backwards * 5.0;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::D))
		velocity += orientation * Vector3::Right * 5.0;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::A))
		velocity += orientation * Vector3::Left * 5.0;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::E))
		velocity += orientation * Vector3::Up * 5.0;

	if (keyboard->IsKeyPressed(Input::KeyboardKey::Q))
		velocity += orientation * Vector3::Down * 5.0;

	_cameraPosition += velocity * deltaTime;

	Matrix4x4 transform = Matrix4x4::CreateTransform(_cameraPosition, orientation, Vector3::One);

	_camera->SetViewMatrix(transform.Inverted());
	//_camera->SetViewMatrix(Matrix4x4::CreateLookAtMatrix(_cameraPosition, _cameraPosition + transform.GetForwardVector(), transform.GetUpVector()));
}
