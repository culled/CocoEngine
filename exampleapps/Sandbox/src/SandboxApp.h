#pragma once
#include <Coco/Core/Application.h>
#include <Coco/Core/Core.h>

using namespace Coco;

class SandboxApp : public Application
{
private:
	TickListener _tickListener;

	EventHandler<const Vector2Int&> _positionChangedHandler;
	EventHandler<const SizeInt&> _sizeChangedHandler;
	EventHandler<uint16> _dpiChangedHandler;
	EventHandler<bool> _focusChangedHandler;

public:
	SandboxApp();
	~SandboxApp();

	void Tick(const TickInfo& tickInfo);
};

