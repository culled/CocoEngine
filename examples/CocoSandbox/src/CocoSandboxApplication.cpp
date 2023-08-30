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
#include <Coco/ECS/Components/RectTransformComponent.h>
#include <Coco/ECS/Components/SpriteRendererComponent.h>
#include <Coco/ECS/Scene.h>

#include "HelloTriangleRenderPass.h"
#include "UIRenderPass.h"

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication() : 
	Coco::Application("Coco Sandbox")
{
	Engine* engine = Engine::Get();

	SharedRef<Logging::ConsoleLogSink> consoleSink = CreateSharedRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	_logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	SharedRef<Logging::FileLogSink> fileSink = CreateSharedRef<Logging::FileLogSink>(Logging::LogLevel::Trace, "coco.log");
	_logger->AddSink(fileSink);
	engine->GetLogger()->AddSink(fileSink);

	_tickListener = engine->GetMainLoop()->CreateTickListener(this, &CocoSandboxApplication::Tick, 0);
	_renderTickListener = engine->GetMainLoop()->CreateTickListener(this, &CocoSandboxApplication::RenderTick, 10000);

	_inputService = engine->GetServiceManager()->CreateService<Input::InputService>();

	Rendering::GraphicsPlatformCreationParameters createParams(Name, Rendering::RenderingRHI::Vulkan);
	_renderService = engine->GetServiceManager()->CreateService<Rendering::RenderingService>(createParams);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	// Setup our basic shader
	_shader = engine->GetResourceLibrary()->Load<Shader>("shaders/built-in/ObjectShader.cshader");
	_material = engine->GetResourceLibrary()->Load<Material>("materials/testMaterial.cmaterial");

	//_material = CreateRef<Material>(_shader);
	//_material->SetColor("_BaseColor", Color::White);
	//_material->SetTexture("_MainTex", _texture);

	// Setup our basic mesh
	const double size = 30.0;
	
	List<VertexData> vertices;
	List<uint> vertexIndices;
	
	MeshPrimitives::CreateXYGrid(Vector2(size, size), Vector3(0.0, 0.0, -size * 0.5), vertices, vertexIndices);
	MeshPrimitives::CreateXZGrid(Vector2(size, size), Vector3(0.0, -size * 0.5, 0.0), vertices, vertexIndices);
	MeshPrimitives::CreateYZGrid(Vector2(size, size), Vector3(-size * 0.5, 0.0, 0.0), vertices, vertexIndices);
	MeshPrimitives::CreateBox(Vector3::One, Vector3(0.0, 5.0, 0.0), vertices, vertexIndices);
	MeshPrimitives::CreateCone(2.0, 1.0, 12, Vector3(2.0, 5.0, 0.0), vertices, vertexIndices);
	MeshPrimitives::CreateUVSphere(12, 12, 0.5, Vector3(-2.0, 5.0, 0.0), vertices, vertexIndices);
	
	_mesh = MeshPrimitives::CreateFromVertices("Mesh", vertices, vertexIndices, false, true);
	_mesh->EnsureChannels(true, true, true, true);
	_mesh->UploadData();

	// Setup our render pipeline
	Ref<Rendering::RenderPipeline> pipeline = engine->GetResourceLibrary()->CreateResource<Rendering::RenderPipeline>("Pipeline");
	
	List<int> attachmentMapping = { 0, 1 };
	_mainRenderPass = CreateSharedRef<HelloTriangleRenderPass>();
	pipeline->AddRenderPass(_mainRenderPass, attachmentMapping);
	pipeline->AddRenderPass(CreateSharedRef<UIRenderPass>(), { 0 });
	_renderService->SetDefaultPipeline(pipeline);

	// Setup our entities
	_ecsService = engine->GetServiceManager()->CreateService<ECS::ECSService>();
	_cameraEntityID = _ecsService->CreateEntity("Camera");

	auto& transform = _ecsService->AddComponent<ECS::TransformComponent>(_cameraEntityID);

	ECS::CameraComponent& camera = _ecsService->AddComponent<ECS::CameraComponent>(_cameraEntityID);
	camera.SetPerspectiveProjection(90.0, 1.0, 0.1, 100.0);
	camera.SetClearColor(Color(0.1, 0.2, 0.3));

	ECS::EntityID obj = _ecsService->CreateEntity("1");
	_ecsService->AddComponent<ECS::TransformComponent>(obj);
	_ecsService->AddComponent<ECS::MeshRendererComponent>(obj, _mesh, _material);

	ECS::EntityID obj2 = _ecsService->CreateEntity("2");
	auto& obj2Transform = _ecsService->AddComponent<ECS::TransformComponent>(obj2);
	obj2Transform.SetPositionAndRotation(Vector3(0, 30, 0), Quaternion(Vector3::Up, Math::Deg2Rad(180)));

	Ref<Mesh> box = MeshPrimitives::CreateBox("Box", Vector3::One * 5.0);
	box->EnsureChannels(true, true, true, true);
	box->UploadData();
	_ecsService->AddComponent<ECS::MeshRendererComponent>(obj2, box, _material);

	_ecsService->GetEntity(obj2).SetParentID(_cameraEntityID);
	_obj2ID = obj2;

	//_spriteShader = engine->GetResourceLibrary()->Load<Shader>("shaders/built-in/UIShader.cshader");
	_spriteMaterial = engine->GetResourceLibrary()->Load<Material>("materials/testUIMaterial.cmaterial");

	_spriteEntityID = _ecsService->CreateEntity("Sprite");
	_ecsService->AddComponent<ECS::RectTransformComponent>(_spriteEntityID, Vector2(50.0, 50.0), 0.0, Size(50.0, 50.0), Vector2(0.5, 0.5));
	_ecsService->AddComponent<ECS::SpriteRendererComponent>(_spriteEntityID, _spriteMaterial);

	_renderService->AddSceneDataProvider(_ecsService->GetRootScene());

	LogInfo(_logger, "Sandbox application created");
}

CocoSandboxApplication::~CocoSandboxApplication()
{
	Engine::Get()->GetMainLoop()->RemoveTickListener(_tickListener);
	Engine::Get()->GetMainLoop()->RemoveTickListener(_renderTickListener);

	LogInfo(_logger, "Sandbox application destroyed");
}

void CocoSandboxApplication::Start()
{
	Windowing::WindowCreateParameters windowCreateParams("Coco Sandbox", SizeInt(1280, 720));

	_window = _windowService->CreateNewWindow(windowCreateParams);
	_window->Show();

	//windowCreateParams.Title = "Child";
	//windowCreateParams.Parent = _window;
	//windowCreateParams.InitialSize = SizeInt(640, 480);
	//windowCreateParams.InitialPosition = Vector2Int::Zero;
	//Ref<Windowing::Window> child = _windowService->CreateNewWindow(windowCreateParams);
	//child->Show();

	_inputService->GetKeyboard()->OnKeyPressedEvent += [&](Input::KeyboardKey key) {
		if (key == Input::KeyboardKey::Escape)
		{
			Quit();
			return true;
		}

		if (key == Input::KeyboardKey::D1)
		{
			switch (_mainRenderPass->GetRenderMode())
			{
			case HelloTriangleRenderPass::RenderModeType::Default:
				_mainRenderPass->SetRenderMode(HelloTriangleRenderPass::RenderModeType::Normals);
				break;
			case HelloTriangleRenderPass::RenderModeType::Normals:
				_mainRenderPass->SetRenderMode(HelloTriangleRenderPass::RenderModeType::Lighting);
				break;
			case HelloTriangleRenderPass::RenderModeType::Lighting:
				_mainRenderPass->SetRenderMode(HelloTriangleRenderPass::RenderModeType::Default);
				break;
			default:
				break;
			}
			return true;
		}

		return false;
		};

	LogInfo(_logger, "Sandbox application started");
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
	
	cameraTransform.SetPositionAndRotation(
		cameraTransform.GetLocalPosition() + velocity * deltaTime,
		orientation);

	//const double t = Coco::Engine::Get()->GetMainLoop()->GetRunningTime();
	//const double a = Math::Sin(t) * 0.5 + 0.5;
	//_materialInstance->SetColor("_BaseColor", Color(a, a, a, 1.0));
	
	//if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Space))
	//{
	//	_textureIndex = (_textureIndex + 1) % static_cast<uint>(s_textureFiles.size());
	//	_texture = Engine::Get()->GetResourceLibrary()->Load<Texture>(s_textureFiles.at(_textureIndex));
	//	_material->SetTexture("_MainTex", _texture);
	//}

	if (keyboard->WasKeyJustPressed(Input::KeyboardKey::F))
	{
		auto& obj2Transform = _ecsService->GetComponent<ECS::TransformComponent>(_obj2ID);
		obj2Transform.SetGlobalPosition(obj2Transform.GetGlobalPosition() + Vector3(0.0, 10.0, 0.0));
	}

	auto& spriteTransform = _ecsService->GetComponent<ECS::RectTransformComponent>(_spriteEntityID);
	spriteTransform.SetLocalRotation(Engine::Get()->GetRunningTime().GetTotalSeconds());
}

void CocoSandboxApplication::RenderTick(double deltaTime)
{
	auto windows = _windowService->GetVisibleWindows();
	
	for (auto& window : windows)
	{
		_renderService->Render(window->GetPresenter(), _ecsService->GetComponent<ECS::CameraComponent>(_cameraEntityID));
	}
}
