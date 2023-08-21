#include "App.h"

#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Engine.h>

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/CameraComponent.h>
#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>

#include "Units/Player.h"
#include "Units/Ball.h"
#include "Units/Block.h"

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

	// Add our ticks
	Engine::Get()->GetMainLoop()->CreateTickListener(this, &App::Tick, 0);
	Engine::Get()->GetMainLoop()->CreateTickListener(this, &App::RenderTick, 1000);

	ConfigureRenderPipeline();

	LogTrace(_logger, "App setup complete");
}

App::~App()
{
	_ecsService->DestroyEntity(_playerEntity);
	_ecsService->DestroyEntity(_ballEntity);

	LogTrace(_logger, "App shutdown");
}

void App::Start()
{
	using namespace Coco::Windowing;

	auto window = _windowingService->CreateNewWindow(WindowCreateParameters(Name, SizeInt(1280, 720)));
	window->Show();

	CreateCamera();
	CreateUnits();
	CreateArena();

	Input::InputService::Get()->GetKeyboard()->OnKeyPressedEvent.AddHandler(this, &App::HandleKeyPressed);
}

void App::ConfigureRenderPipeline()
{
	using namespace Coco::Rendering;
	ResourceLibrary* library = Engine::Get()->GetResourceLibrary();

	Ref<RenderPipeline> renderPipeline = library->CreateResource<RenderPipeline>("RenderPipeline");

	// Color attachment will be index 0, depth at index 1
	renderPipeline->AddRenderPass(CreateSharedRef<OpaqueRenderPass>(), { 0, 1 });

	_renderingService->SetDefaultPipeline(renderPipeline);

	_basicShader = library->CreateResource<Shader>("Basic Shader");

	auto pipelineState = GraphicsPipelineState();

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
			ShaderDescriptor("_BaseColor", BufferDataFormat::Color)
		},
		{
			ShaderTextureSampler("_MainTex")
		}
		);

	_wallMaterial = library->CreateResource<Material>("Material::Wall", _basicShader);
	_wallMaterial->SetColor("_BaseColor", Color(0.55, 0.55, 0.55));
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
	camera.SetClearColor(_clearColor);
}

void App::CreateUnits()
{
	_playerEntity = _ecsService->CreateEntity("Player");
	_ecsService->AddComponent<Player>(_playerEntity);

	_ballEntity = _ecsService->CreateEntity("Ball");
	_ecsService->AddComponent<Ball>(_ballEntity);

	const double blockStartX = -_arenaSize.Width * 0.5 + 1.5;
	const double blockEndX = _arenaSize.Width * 0.5 - 1.5;

	double y = _blockStartingY;
	for (int r = 0; r < _blockRows; r++)
	{
		Ref<Rendering::Material> blockRowMaterial = Engine::Get()->GetResourceLibrary()->CreateResource<Material>(FormattedString("Block Row {} Material", r), _basicShader);
		blockRowMaterial->SetColor("_BaseColor", _blockRowColors.at(r));
		blockRowMaterial->SetTexture("_MainTex", GetRenderingService()->GetDefaultDiffuseTexture());

		for (double x = blockStartX; x <= blockEndX; x += 2.0)
		{
			EntityID block = _ecsService->CreateEntity("Block");
			_ecsService->AddComponent<Block>(block, Vector3(x, y, 0.0), blockRowMaterial->ID, _blockRowSpeeds.at(r));
			_blockEntities.Add(block);
		}

		y += 0.5;
	}
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

	_renderingService->AddSceneDataProvider(_ecsService->GetRootScene());
}

void App::StartGame()
{
	OnStartPlaying.Invoke();
}

void App::Tick(double deltaTime)
{
	CheckForCollisions();
}

void App::RenderTick(double deltaTime)
{
	_renderingService->Render(_windowingService->GetMainWindow()->GetPresenter(), _ecsService->GetComponent<CameraComponent>(_cameraEntity));
}

bool App::HandleKeyPressed(Input::KeyboardKey key)
{
	if (!_isPlaying && key == Input::KeyboardKey::Space)
	{
		StartGame();
		return true;
	}

	return false;
}

void App::CheckForCollisions()
{
	Ball& ball = _ecsService->GetComponent<Ball>(_ballEntity);
	Rect ballRect = ball.GetRect();
	Vector2 normal;
	Vector2 hitPoint;

	if (CollidedWithArena(ballRect, hitPoint, normal))
	{
		ball.Bounce(hitPoint, normal);
	}

	List<EntityID> blocks(_blockEntities);
	for (const EntityID& block : blocks)
	{
		Block& b = _ecsService->GetComponent<Block>(block);
		if (b.CheckCollision(ballRect, hitPoint, normal))
		{
			_ecsService->QueueDestroyEntity(block);
			_blockEntities.Remove(block);
			ball.Bounce(hitPoint, normal);
			ball.SpeedUp(b.GetSpeedValue());
		}
	}

	Player& player = _ecsService->GetComponent<Player>(_playerEntity);
	Rect playerRect = player.GetRect();
	if (ballRect.Intersects(playerRect))
	{
		Rect::RectangleSide side;
		hitPoint = playerRect.GetClosestPoint(ballRect, &side);
		normal = Vector2::Up;
		double bentX = (ballRect.GetCenter() - playerRect.GetCenter()).X / playerRect.Size.Width;
		normal.X += bentX * 0.6;
		normal.Normalize();

		if (normal.Dot(ball.GetMovementDirection()) > -0.1)
		{
			normal = Vector2::Up;
		}

		ball.Bounce(hitPoint, normal);
	}
}

bool App::CollidedWithArena(const Rect& rect, Vector2& hitPoint, Vector2& hitNormal) const
{
	Vector3 lowerLeft(_arenaOffset + Vector3::Left * (_arenaSize.Width * 0.5 + 0.5) - Vector3::Forwards * _arenaSize.Height * 0.5);
	Rect leftWall(Vector2(lowerLeft.X, lowerLeft.Y), Size(1.0, _arenaSize.Height));
	Rect rightWall(leftWall.Offset + Vector2::Right * _arenaSize.Width, leftWall.Size);
	Rect ceiling(leftWall.Offset + Vector2::Up * (_arenaSize.Height - 1.0), Size(_arenaSize.Width, 1.0));

	if (leftWall.Intersects(rect))
	{
		hitPoint = leftWall.GetClosestPoint(rect);
		hitNormal = Vector3::Right;
		return true;
	}
	else if (rightWall.Intersects(rect))
	{
		hitPoint = rightWall.GetClosestPoint(rect);
		hitNormal = Vector3::Left;
		return true;
	}
	else if (ceiling.Intersects(rect))
	{
		hitPoint = ceiling.GetClosestPoint(rect);
		hitNormal = Vector3::Backwards;
		return true;
	}

	return false;
}