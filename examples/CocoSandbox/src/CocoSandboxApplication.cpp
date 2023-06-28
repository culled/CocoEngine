#include "CocoSandboxApplication.h"

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Logging/Sinks/FileLogSink.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include <Coco/ECS/ECSService.h>
#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/CameraComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/ECS/Scene.h>

#include "HelloTriangleRenderPass.h"

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication(Coco::Engine* engine) : 
	Coco::Application(engine, "Coco Sandbox")
{
	SharedRef<Logging::ConsoleLogSink> consoleSink = CreateSharedRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	SharedRef<Logging::FileLogSink> fileSink = CreateSharedRef<Logging::FileLogSink>(Logging::LogLevel::Trace, "coco.log");
	Logger->AddSink(fileSink);
	engine->GetLogger()->AddSink(fileSink);

	_inputService = engine->GetServiceManager()->CreateService<Input::InputService>();

	Rendering::GraphicsPlatformCreationParameters createParams(Name, Rendering::RenderingRHI::Vulkan);
	_renderService = engine->GetServiceManager()->CreateService<Rendering::RenderingService>(createParams);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	// Setup our basic shader
	_shader = engine->GetResourceLibrary()->Load<Shader>(ResourceLibrary::DefaultTickLifetime, "shaders/built-in/ObjectShader.cshader");
	_texture = engine->GetResourceLibrary()->Load<Texture>(ResourceLibrary::DefaultTickLifetime, s_textureFiles.at(0));
	_material = engine->GetResourceLibrary()->Load<Material>(ResourceLibrary::DefaultTickLifetime, "materials/testMaterial.cmaterial");

	//_material = CreateRef<Material>(_shader);
	//_material->SetVector4("_BaseColor", Color::White);
	//_material->SetTexture("_MainTex", _texture);

	// Setup our basic mesh
	const double size = 30.0;
	
	List<Vector3> vertexPositions;
	List<Vector2> vertexUVs;
	List<uint> vertexIndices;
	
	MeshPrimitives::CreateXYGrid(Vector2(size, size), Vector3(0.0, 0.0, -size * 0.5), vertexPositions, vertexUVs, vertexIndices);
	MeshPrimitives::CreateXZGrid(Vector2(size, size), Vector3(0.0, -size * 0.5, 0.0), vertexPositions, vertexUVs, vertexIndices);
	MeshPrimitives::CreateYZGrid(Vector2(size, size), Vector3(-size * 0.5, 0.0, 0.0), vertexPositions, vertexUVs, vertexIndices);
	MeshPrimitives::CreateBox(Vector3::One, Vector3(0.0, 5.0, 0.0), vertexPositions, vertexUVs, vertexIndices);
	
	_mesh = MeshPrimitives::CreateFromVertices(vertexPositions, vertexUVs, vertexIndices);

	// Setup our render pipeline
	Ref<Rendering::RenderPipeline> pipeline = engine->GetResourceLibrary()->CreateResource<Rendering::RenderPipeline>("Pipeline", ResourceLibrary::DefaultTickLifetime);
	pipeline->SetClearColor(Color(0.1, 0.2, 0.3));
	
	List<int> attachmentMapping = { 0, 1 };
	
	pipeline->AddRenderPass(CreateSharedRef<HelloTriangleRenderPass>(), attachmentMapping);
	_renderService->SetDefaultPipeline(pipeline);

	// Setup our entities
	_ecsService = engine->GetServiceManager()->CreateService<ECS::ECSService>();
	_cameraEntityID = _ecsService->CreateEntity("Camera");

	_ecsService->AddComponent<ECS::TransformComponent>(_cameraEntityID);

	ECS::CameraComponent& camera = _ecsService->AddComponent<ECS::CameraComponent>(_cameraEntityID);
	camera.SetPerspectiveProjection(90.0, 1.0, 0.1, 100.0);

	ECS::EntityID obj = _ecsService->CreateEntity("1");
	_ecsService->AddComponent<ECS::TransformComponent>(obj);
	_ecsService->AddComponent<ECS::MeshRendererComponent>(obj, _mesh, _material);

	ECS::EntityID obj2 = _ecsService->CreateEntity("2");
	auto& obj2Transform = _ecsService->AddComponent<ECS::TransformComponent>(obj2);
	obj2Transform.SetLocalPosition(Vector3(0, 30, 0));
	obj2Transform.SetLocalRotation(Quaternion(Vector3::Up, Math::Deg2Rad(180)));

	_ecsService->AddComponent<ECS::MeshRendererComponent>(obj2, _mesh, _material);

	//_ecsService->GetEntity(obj2).SetParentID(_cameraEntityID);

	_tickListener = Engine->GetMainLoop()->CreateTickListener(this, &CocoSandboxApplication::Tick, 0);
	_renderTickListener = Engine->GetMainLoop()->CreateTickListener(this, &CocoSandboxApplication::RenderTick, 10000);

	LogInfo(Logger, "Sandbox application created");
}

CocoSandboxApplication::~CocoSandboxApplication()
{
	this->Engine->GetMainLoop()->RemoveTickListener(_tickListener);
	this->Engine->GetMainLoop()->RemoveTickListener(_renderTickListener);

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

	LogInfo(Logger, "Sandbox application started");
}

void CocoSandboxApplication::Tick(double deltaTime)
{
	Vector2 mouseDelta = _inputService->GetMouse()->GetDelta();

	ECS::TransformComponent& cameraTransform = _ecsService->GetComponent<ECS::TransformComponent>(_cameraEntityID);

	Vector3 cameraEulerAngles = cameraTransform.GetLocalEulerAngles();
	cameraEulerAngles.Z -= mouseDelta.X * 0.005;
	cameraEulerAngles.X = Math::Clamp(cameraEulerAngles.X - mouseDelta.Y * 0.005, Math::Deg2Rad(-90.0), Math::Deg2Rad(90.0));

	cameraTransform.SetLocalEulerAngles(cameraEulerAngles);

	Quaternion orientation = cameraTransform.GetLocalRotation();

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

	
	cameraTransform.SetLocalPosition(cameraTransform.GetLocalPosition() + velocity * deltaTime);
	cameraTransform.SetLocalRotation(orientation);

	ECS::CameraComponent& camera = _ecsService->GetComponent<ECS::CameraComponent>(_cameraEntityID);
	camera.SetViewMatrix(cameraTransform.GetGlobalTransformMatrix().Inverted());

	const double t = Coco::Engine::Get()->GetMainLoop()->GetRunningTime();
	const double a = Math::Sin(t) * 0.5 + 0.5;
	_material->SetVector4("_BaseColor", Color(a, a, a, 1.0));
	
	if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Space))
	{
		_textureIndex = (_textureIndex + 1) % static_cast<uint>(s_textureFiles.size());
		_texture = Engine->GetResourceLibrary()->Load<Texture>(ResourceLibrary::DefaultTickLifetime, s_textureFiles.at(_textureIndex));
		_material->SetTexture("_MainTex", _texture);
	}

	// Update mesh data on the GPU if it is dirty
	if (_mesh->GetIsDirty())
		_mesh->UploadData();
}

void CocoSandboxApplication::RenderTick(double deltaTime)
{
	auto windows = _windowService->GetRenderableWindows();
	
	for (auto& window : windows)
	{
		_renderService->Render(window->GetPresenter(), &_ecsService->GetComponent<ECS::CameraComponent>(_cameraEntityID), _ecsService->GetRootScene());
	}
}
