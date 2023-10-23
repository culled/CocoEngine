#include "Corepch.h"
#include "TickListener.h"

#include "../Engine.h"

namespace Coco
{
	TickListener::TickListener(CallbackFunction callback, int priority) :
		_callback(callback),
		Priority(priority),
		_enabled(true),
		_tickPeriod(0.0),
		_timeSinceLastTick(0.0),
		_isRegistered(false)
	{}

	void TickListener::SetEnabled(bool enabled)
	{
		_enabled = enabled;
	}

	void TickListener::SetTickPeriod(double period)
	{
		_tickPeriod = period;
	}

	void TickListener::SetCallback(CallbackFunction callback)
	{
		_callback = callback;
	}

	void TickListener::Invoke(const TickInfo& tickInfo)
	{
		if (!_enabled)
			return;

		_timeSinceLastTick += tickInfo.DeltaTime;

		if (_timeSinceLastTick >= _tickPeriod)
		{
			if (_callback)
				_callback(tickInfo);

			_timeSinceLastTick = 0.0;
		}
	}
}