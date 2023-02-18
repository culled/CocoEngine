#include "MainLoop.h"

#include <Coco/Core/Platform/EnginePlatform.h>

#include "MainLoopTickListener.h"

namespace Coco
{
	MainLoop::MainLoop(Platform::EnginePlatform* platform) :
		_platform(platform)
	{
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		_currentTickTime = _platform->GetPlatformTimeSeconds();
		_lastTickTime = _currentTickTime;

		double preTickTime = _currentTickTime;
		bool didTick = true;

		while (_isRunning)
		{
			// Only record the time once each loop
			// Without this, we wouldn't account for time spent suspended as the loop only partially runs while suspended
			if (didTick)
			{
				preTickTime = _platform->GetPlatformTimeSeconds();
				didTick = false;
			}

			_platform->HandlePlatformMessages();

			// Only process messages if suspended
			if (_isSuspended)
			{
				// TODO: configurable sleep time?
				_platform->Sleep(1);
				continue;
			}

			if (_tickListenersNeedSorting)
				SortTickListeners();

			_currentTickTime = _platform->GetPlatformTimeSeconds();
			
			// Calculate time since the last tick
			_currentUnscaledDeltaTime = _currentTickTime - _lastTickTime;

			// Remove the time spent processing messages/suspended, preventing massive delta times while caught in a message process loop
			if (!_useAbsoluteTiming)
				_currentUnscaledDeltaTime -= _currentTickTime - preTickTime;

			_currentUnscaledRunningTime += _currentUnscaledDeltaTime;

			_currentDeltaTime = _currentUnscaledDeltaTime * _timeScale;
			_currentRunningTime += _currentDeltaTime;

			List<Ref<MainLoopTickListener>> listenersCopy = _tickListeners;

			for (const Ref<MainLoopTickListener>& listener : listenersCopy)
			{
				listener->Tick(_currentDeltaTime);
			}

			_lastTickTime = _currentTickTime;
			_tickCount++;
			didTick = true;

			if (_targetTickRate > 0 && _isRunning)
				WaitForNextTick();
		}
	}

	void MainLoop::Stop()
	{
		_isRunning = false;
	}

	void MainLoop::SetIsSuspended(bool isSuspended)
	{
		_isSuspended = isSuspended;
	}

	void MainLoop::AddTickListener(Ref<MainLoopTickListener> tickListener)
	{
		_tickListeners.Add(tickListener);
		_tickListenersNeedSorting = true;
	}

	void MainLoop::RemoveTickListener(const Ref<MainLoopTickListener>& tickListener)
	{
		_tickListeners.Remove(tickListener);
	}

	bool MainLoop::CompareTickListeners(const Ref<MainLoopTickListener>& a, const Ref<MainLoopTickListener>& b)
	{
		// If true, a gets placed before b
		return a->Priority < b->Priority;
	}

	void MainLoop::SortTickListeners()
	{
		std::sort(_tickListeners.begin(), _tickListeners.end(), CompareTickListeners);
		_tickListenersNeedSorting = false;
	}

	void MainLoop::WaitForNextTick()
	{
		if (_targetTickRate <= 0)
			return;

		double nextTickTime = _currentTickTime + (1.0 / _targetTickRate);

		double timeRemaining = nextTickTime - _platform->GetPlatformTimeSeconds();
		double estimatedWait = 0;
		int waitCount = 1;

		// Sleep until we feel like sleeping would overshoot our target time
		while (timeRemaining > estimatedWait)
		{
			double waitStartTime = _platform->GetPlatformTimeSeconds();

			_platform->Sleep(1);

			double waitTime = _platform->GetPlatformTimeSeconds() - waitStartTime;
			timeRemaining -= waitTime;

			double delta = waitTime - estimatedWait;

			estimatedWait += delta / waitCount;
			waitCount++;
		}

		// Actively wait until our next tick time
		while (_platform->GetPlatformTimeSeconds() < nextTickTime)
		{}
	}
}