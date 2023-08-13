#pragma once

#include <Coco/Core/Application.h>
#include <Coco/ECS/Scene.h>
#include <Coco/Windowing/Window.h>
#include <Coco/ECS/EntityTypes.h>
#include <Coco/ECS/ECSService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Core/Events/Event.h>

using namespace Coco;

class App : public Application
{
public:
	Event<> OnStartPlaying;
	Event<> OnStopPlaying;

private:
	static App* _instance;

	Ref<Rendering::Shader> _basicShader;
	Ref<Rendering::Material> _wallMaterial;

	ECS::ECSService* _ecsService;
	Input::InputService* _inputService;
	Rendering::RenderingService* _renderingService;
	Windowing::WindowingService* _windowingService;

	ECS::EntityID _playerEntity;
	ECS::EntityID _ballEntity;
	List<ECS::EntityID> _blockEntities;

	Color _clearColor = Color(0.1, 0.15, 0.2);
	double _cameraOrthoSize = 20.0;
	Vector3 _cameraPosition = Vector3(0.0, 0.0, 10.0);
	Vector3 _cameraRotation = Vector3(Math::Deg2Rad(-90.0), 0.0, 0.0);
	ECS::EntityID _cameraEntity;

	Size _arenaSize = Size(35.0, 16.0);
	Vector3 _arenaOffset = Vector3(0.0, 0.0, 1.0);

	int _blockRows = 6;
	double _blockStartingY = 2.0;
	Array<Color, 6> _blockRowColors = {
		Color(0.27, 0.27, 0.77),
		Color(0.27, 0.62, 0.28),
		Color(0.64, 0.62, 0.17),
		Color(0.71, 0.48, 0.19),
		Color(0.78, 0.42, 0.23),
		Color(0.78, 0.29, 0.27) 
	};
	Array<double, 6> _blockRowSpeeds = {
		8.0,
		8.5,
		9.5,
		11.0,
		13.0,
		16.0
	};

	bool _isPlaying = false;

public:
	App();
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

	Ref<Rendering::Shader> GetBasicShader() { return _basicShader; }

private:
	void ConfigureRenderPipeline();
	void CreateCamera();
	void CreateUnits();
	void CreateArena();

	void StartGame();

	void Tick(double deltaTime);
	void RenderTick(double deltaTime);

	bool HandleKeyPressed(Input::KeyboardKey key);

	void CheckForCollisions();
	bool CollidedWithArena(const Rect& rect, Vector2& hitPoint, Vector2& hitNormal) const;
};