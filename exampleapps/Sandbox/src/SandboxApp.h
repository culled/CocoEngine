#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>

using namespace Coco;

class SandboxApp : public Application
{
private:
	TickListener _tickListener;

public:
	SandboxApp();
	~SandboxApp();

protected:
	void Start() final;

private:
	void Tick(const TickInfo& tickInfo);
};

