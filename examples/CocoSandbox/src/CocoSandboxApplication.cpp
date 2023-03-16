#include "CocoSandboxApplication.h"

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Logging/Sinks/FileLogSink.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/MeshPrimitives.h>

MainApplication(CocoSandboxApplication)

using namespace Coco;

ShaderUniformObject::ShaderUniformObject() : Padding{ 0 }
{
	BaseColor[0] = 0.0f;
	BaseColor[1] = 0.0f;
	BaseColor[2] = 0.0f;
	BaseColor[3] = 1.0f;
}

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

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	_camera = CreateRef<CameraComponent>();
	_camera->SetPerspectiveProjection(90.0, 1.0, 0.1, 100.0);
	//_camera->SetOrthographicProjection(10.0, 1.0, 0.1, 100.0);
	_cameraPosition = Vector3(0.0, 0.0, 0.0);

	//engine->GetMainLoop()->SetTargetTickRate(2);

	// Setup our basic shader
	GraphicsPipelineState pipelineState;
	//pipelineState.CullingMode = CullMode::None;
	_shader = CreateRef<Shader>("HelloTriangle");
	_shader->CreateSubshader("main",
		{
			{ShaderStageType::Vertex, "assets/shaders/built-in/ObjectShader.vert.spv"},
			{ShaderStageType::Fragment, "assets/shaders/built-in/ObjectShader.frag.spv"}
		},
		pipelineState,
		{
			ShaderVertexAttribute(BufferDataFormat::Vector3),
			ShaderVertexAttribute(BufferDataFormat::Vector2)
		},
		{
			ShaderDescriptor("_MaterialInfo", 0, ShaderDescriptorType::UniformStruct, sizeof(ShaderUniformObject)),
			ShaderDescriptor("_MainTex", 1, ShaderDescriptorType::UniformSampler)
		});

	_texture = CreateRef<Texture>(s_textureFiles.at(0), ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled);

	_material = CreateRef<Material>(_shader);
	_material->SetStruct("_MaterialInfo", _shaderUO);
	_material->SetTexture("_MainTex", _texture);

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

	//_mesh = MeshPrimitives::CreateBox(Vector3::One * size);

	_meshTransform = Matrix4x4::CreateWithTranslation(Vector3(0.0, 0.0, 0.0));

	List<int> attachmentMapping;
	attachmentMapping.Add(0);

	_rp = CreateRef<HelloTriangleRenderPass>(_mesh, _meshTransform, _material);
	pipeline->AddRenderPass(_rp, attachmentMapping);
	_renderService->SetDefaultPipeline(pipeline);

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

	const double t = Coco::Engine::Get()->GetMainLoop()->GetRunningTime();
	const double a = Math::Sin(t) * 0.5 + 0.5;
	_shaderUO.BaseColor[0] = static_cast<float>(a);
	_shaderUO.BaseColor[1] = static_cast<float>(a);
	_shaderUO.BaseColor[2] = static_cast<float>(a);
	_material->SetStruct("_MaterialInfo", _shaderUO);

	if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Space))
	{
		_textureIndex = (_textureIndex + 1) % static_cast<uint>(s_textureFiles.size());
		_texture->LoadFromFile(s_textureFiles.at(_textureIndex));
	}

	_rp->UpdateMeshTransform(_meshTransform);
}
