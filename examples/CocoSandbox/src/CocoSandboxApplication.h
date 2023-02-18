#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Events/Query.h>
#include <Coco/Core/Events/Event.h>

class Coco::Engine;

namespace Coco::Windowing
{
	class WindowingService;
	class Window;
}

namespace Coco::Input
{
	class InputService;
}

namespace Coco
{
	class MainLoopTickListener;
}

class CocoSandboxApplication : public Coco::Application
{
private:
	Coco::Windowing::WindowingService* _windowService;
	Coco::Windowing::Window* _window;
	Coco::Input::InputService* _inputService;
	Coco::Ref<Coco::MainLoopTickListener> _tickListener;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	virtual ~CocoSandboxApplication() override;

	virtual void Start() override;

	void Tick(double deltaTime);
};

