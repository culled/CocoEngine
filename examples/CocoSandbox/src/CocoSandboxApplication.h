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

class CocoSandboxApplication : public Coco::Application
{
private:
	Coco::Windowing::WindowingService* _windowService;
	Coco::Windowing::Window* _window;
	Coco::Query<int, int> _query;
	Coco::Event<int&> _event;

public:
	CocoSandboxApplication(Coco::Engine* engine);
	virtual ~CocoSandboxApplication() override;

	virtual void Start() override;

private:
	int QueryHandler(int param);
	bool EventHandler(int& param);
};

