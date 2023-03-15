#include "MainLoopTickListener.h"

namespace Coco
{
	MainLoopTickListener::MainLoopTickListener(TickHandler tickHandler, int priority) :
		Priority(priority), _handler(tickHandler), _isEnabled(true)
	{}

	void MainLoopTickListener::Tick(double deltaTime)
	{
		if (_handler && _isEnabled)
			_handler(deltaTime);
	}
}