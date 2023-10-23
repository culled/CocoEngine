#include "Corepch.h"
#include "Stopwatch.h"

#include "../Engine.h"

namespace Coco
{
	Stopwatch::Stopwatch() :
		Stopwatch(false)
	{}

	Stopwatch::Stopwatch(bool startAutomatically) :
		_time(),
		_startTimeSeconds(0.0),
		_isRunning(false)
	{
		if (startAutomatically)
			Start();
	}

	void Stopwatch::Start()
	{
		if (_isRunning)
			return;

		_startTimeSeconds = Engine::cGet()->GetPlatform().GetSeconds();
		_isRunning = true;
	}

	const TimeSpan& Stopwatch::Stop()
	{
		if (_isRunning)
		{
			_time = TimeSpan::FromSeconds(Engine::cGet()->GetPlatform().GetSeconds() - _startTimeSeconds);
			_isRunning = false;
		}

		return _time;
	}
}