#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Events/Query.h>
#include <Coco/Core/Events/Event.h>

#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco
{
	class Engine;
	class MainLoopTickListener;
}

namespace Coco::Windowing
{
	class WindowingService;
	class Window;
}

namespace Coco::Input
{
	class InputService;
}

namespace Coco::Rendering
{
	class RenderingService;
}

class CocoSandboxApplication final : public Coco::Application
{
private:
	Coco::Ref<Coco::MainLoopTickListener> _tickListener;
	Coco::Input::InputService* _inputService;
	Coco::Rendering::RenderingService* _renderService;
	Coco::Windowing::WindowingService* _windowService;
	Coco::Windowing::Window* _window = nullptr;
	Coco::Ref<Coco::CameraComponent> _camera;
	Coco::Vector3 _cameraPosition;
	Coco::Vector3 _cameraEulerAngles;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	~CocoSandboxApplication() final;

	void Start() final;
	Coco::Ref<Coco::CameraComponent> GetCamera() const noexcept final { return _camera; }

	void Tick(double deltaTime);
};

