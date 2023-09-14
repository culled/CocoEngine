#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

using namespace Coco;

class SandboxApp : public Application
{
private:
	TickListener _tickListener;
	SharedRef<Rendering::RenderPipeline> _pipeline;

public:
	SandboxApp();
	~SandboxApp();

protected:
	void Start() final;

private:
	void Tick(const TickInfo& tickInfo);
};

