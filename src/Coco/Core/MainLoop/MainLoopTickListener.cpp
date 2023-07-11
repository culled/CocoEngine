#include "MainLoopTickListener.h"

namespace Coco
{
	MainLoopTickListener::MainLoopTickListener(TickHandler tickHandler, int priority, double period) :
		Priority(priority), 
		_handler(tickHandler), 
		_isEnabled(true),
		_period(period),
		_timeSinceLastTick(0.0)
	{}

	void MainLoopTickListener::Tick(double deltaTime)
	{
		if (_handler && _isEnabled)
		{
			if (_timeSinceLastTick + Math::Epsilon >= _period)
			{
				_handler(deltaTime + _timeSinceLastTick);
				_timeSinceLastTick = 0.0;
			}
			else
			{
				_timeSinceLastTick += deltaTime;
			}
		}
	}
}