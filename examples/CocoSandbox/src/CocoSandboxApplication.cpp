#include "CocoSandboxApplication.h"

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Logging/Sinks/FileLogSink.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include <Coco/Core/Scene/Components/TransformComponent.h>
#include <Coco/Rendering/Components/CameraComponent.h>
#include <Coco/Rendering/Components/MeshRendererComponent.h>

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

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	// Setup our basic shader
	_shader = RefCast<Shader>(
		engine->GetResourceLibrary()->GetOrLoadResource(ResourceType::Shader, "shaders/built-in/ObjectShader.cshader")
		);
	_texture = RefCast<Texture>(
		engine->GetResourceLibrary()->GetOrLoadResource(ResourceType::Texture, s_textureFiles.at(0))
		);
	_material = RefCast<Material>(
		engine->GetResourceLibrary()->GetOrLoadResource(ResourceType::Material, "materials/testMaterial.cmaterial")
		);

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

	_obj = CreateRef<SceneEntity>();
	_obj->AddComponent<TransformComponent>();
	_obj->AddComponent<MeshRendererComponent>(_mesh, _material);
	Scene->AddEntity(_obj);

	_obj2 = CreateRef<SceneEntity>();
	TransformComponent* obj2Transform = _obj2->AddComponent<TransformComponent>();
	obj2Transform->SetPosition(Vector3(0, 30, 0));
	obj2Transform->SetRotation(Quaternion(Vector3::Up, Math::Deg2Rad(180)));
	_obj2->AddComponent<MeshRendererComponent>(_mesh, _material);
	Scene->AddEntity(_obj2);

	// Setup our render pipeline
	Ref<Rendering::RenderPipeline> pipeline = CreateRef<Rendering::RenderPipeline>();
	pipeline->SetClearColor(Color(0.1, 0.2, 0.3));

	List<int> attachmentMapping = { 0, 1 };

	pipeline->AddRenderPass(CreateRef<HelloTriangleRenderPass>(), attachmentMapping);
	_renderService->SetDefaultPipeline(pipeline);

	_camera = CreateRef<SceneEntity>();
	_camera->AddComponent<TransformComponent>();
	CameraComponent* cameraComponent = _camera->AddComponent<CameraComponent>();
	cameraComponent->SetPerspectiveProjection(90.0, 1.0, 0.1, 100.0);
	//_camera->SetOrthographicProjection(10.0, 1.0, 0.1, 100.0);

	Scene->AddEntity(_camera);

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
	TransformComponent* cameraTransform = _camera->GetComponent<TransformComponent>();
	CameraComponent* camera = _camera->GetComponent<CameraComponent>();

	Vector2 mouseDelta = _inputService->GetMouse()->GetDelta();

	_cameraEulerAngles.Z -= mouseDelta.X * 0.005;
	_cameraEulerAngles.X = Math::Clamp(_cameraEulerAngles.X - mouseDelta.Y * 0.005, Math::Deg2Rad(-90.0), Math::Deg2Rad(90.0));

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

	cameraTransform->SetPosition(cameraTransform->GetPosition() + velocity * deltaTime);
	cameraTransform->SetRotation(orientation);

	camera->SetViewMatrix(cameraTransform->GetTransformMatrix().Inverted());

	const double t = Coco::Engine::Get()->GetMainLoop()->GetRunningTime();
	const double a = Math::Sin(t) * 0.5 + 0.5;
	_material->SetVector4("_BaseColor", Color(a, a, a, 1.0));

	if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Space))
	{
		_textureIndex = (_textureIndex + 1) % static_cast<uint>(s_textureFiles.size());
		_texture = RefCast<Texture>(Engine->GetResourceLibrary()->GetOrLoadResource(ResourceType::Texture, s_textureFiles.at(_textureIndex)));
		_material->SetTexture("_MainTex", _texture);
	}

	// Update mesh data on the GPU if it is dirty
	if (_mesh->GetIsDirty())
		_mesh->UploadData();
}
