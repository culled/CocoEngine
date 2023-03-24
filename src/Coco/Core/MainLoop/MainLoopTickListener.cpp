#include "MainLoopTickListener.h"

namespace Coco
{
	MainLoopTickListener::MainLoopTickListener(TickHandler tickHandler, int priority) :
		Priority(priority), _handler(tickHandler), _isEnabled(true)
	{}

	void MainLoopTickListener::PurgeTick(double deltaTime)
	{
		if (_handler && _isEnabled)
			_handler(deltaTime);
	}
}