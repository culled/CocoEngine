#pragma once

#include <Coco/Core/Application.h>

class Coco::Engine;

namespace Coco::Windowing
{
	class WindowingService;
	class Window;
}

class CocoSandboxApplication : public Coco::Application
{
private:
	Coco::Windowing::WindowingService* _windowService;
	Coco::Windowing::Window* _window;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	virtual ~CocoSandboxApplication() override;

	virtual void Start() override;
};

