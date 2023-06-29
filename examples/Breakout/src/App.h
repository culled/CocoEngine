#pragma once

#include <Coco/Core/Application.h>
#include <Coco/ECS/Scene.h>
#include <Coco/Windowing/Window.h>
#include <Coco/ECS/EntityTypes.h>
#include <Coco/ECS/ECSService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Windowing/WindowingService.h>

using namespace Coco;

class App : public Application
{
private:
	static App* _instance;

	ECS::ECSService* _ecsService;
	Input::InputService* _inputService;
	Rendering::RenderingService* _renderingService;
	Windowing::WindowingService* _windowingService;

	ECS::EntityID _playerEntity;

	Color _clearColor = Color(0.1, 0.15, 0.2);
	double _cameraOrthoSize = 20.0;
	Vector3 _cameraPosition = Vector3(0.0, 0.0, 10.0);
	Vector3 _cameraRotation = Vector3(Math::Deg2Rad(-90.0), 0.0, 0.0);
	ECS::EntityID _cameraEntity;

public:
	App(Coco::Engine* engine);
	~App();

	static App* Get() { return _instance; }

	// Inherited via Application
	virtual void Start() override;

	ECS::ECSService* GetECSService() { return _ecsService; }
	Input::InputService* GetInputService() { return _inputService; }
	Rendering::RenderingService* GetRenderingService() { return _renderingService; }
	Windowing::WindowingService* GetWindowingService() { return _windowingService; }

	Ref<Windowing::Window> GetWindow() { return _windowingService->GetMainWindow(); }
	ECS::Scene* GetScene() { return _ecsService->GetRootScene(); }

private:
	void ConfigureRenderPipeline();
	void CreateCamera();
	void CreatePlayer();

	void RenderTick(double deltaTime);
};