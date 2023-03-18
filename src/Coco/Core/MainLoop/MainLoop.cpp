#include "MainLoop.h"

#include <Coco/Core/Platform/IEnginePlatform.h>

#include "MainLoopTickListener.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	MainLoop::MainLoop(Platform::IEnginePlatform* platform) noexcept :
		_platform(platform)
	{
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		// Set the current time to the current time
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
				_platform->Sleep(s_suspendSleepPeriodMs);
				continue;
			}

			if (_tickListenersNeedSorting)
				SortTickListeners();

			_currentTickTime = _platform->GetPlatformTimeSeconds();
			
			// Calculate time since the last tick
			_currentUnscaledDeltaTime = _currentTickTime - _lastTickTime;

			// Optionally remove the time spent processing messages/suspended, preventing massive delta times while caught in a message process loop
			if (!_useAbsoluteTiming)
				_currentUnscaledDeltaTime -= _currentTickTime - preTickTime;

			_currentUnscaledRunningTime += _currentUnscaledDeltaTime;

			// Scaled delta and running times
			_currentDeltaTime = _currentUnscaledDeltaTime * _timeScale;
			_currentRunningTime += _currentDeltaTime;

			// Copy the tick listeners so that we don't have issues if a listener removes themselves during the tick
			List<Ref<MainLoopTickListener>> listenersCopy = _tickListeners;

			for (const Ref<MainLoopTickListener>& listener : listenersCopy)
			{
				listener->Tick(_currentDeltaTime);
			}

			_lastTickTime = _currentTickTime;
			_tickCount++;
			didTick = true;

			// Optionally sleep if we're throttling the loop
			if (_targetTickRate > 0 && _isRunning)
				WaitForNextTick();
		}
	}

	void MainLoop::Stop() noexcept
	{
		_isRunning = false;
	}

	void MainLoop::SetIsSuspended(bool isSuspended) noexcept
	{
		_isSuspended = isSuspended;
	}

	void MainLoop::AddTickListener(Ref<MainLoopTickListener> tickListener)
	{
		_tickListeners.Add(tickListener);
		_tickListenersNeedSorting = true;
	}

	void MainLoop::RemoveTickListener(const Ref<MainLoopTickListener>& tickListener) noexcept
	{
		try
		{
			_tickListeners.Remove(tickListener);
		}
		catch(...)
		{ }
	}

	bool MainLoop::CompareTickListeners(const Ref<MainLoopTickListener>& a, const Ref<MainLoopTickListener>& b) noexcept
	{
		// If true, a gets placed before b
		return a->Priority < b->Priority;
	}

	void MainLoop::SortTickListeners() noexcept
	{
		std::sort(_tickListeners.begin(), _tickListeners.end(), CompareTickListeners);
		_tickListenersNeedSorting = false;
	}

	void MainLoop::WaitForNextTick()
	{
		if (_targetTickRate <= 0)
			return;

		const double nextTickTime = _currentTickTime + (1.0 / _targetTickRate);

		double timeRemaining = nextTickTime - _platform->GetPlatformTimeSeconds();
		double estimatedWait = 0;
		int waitCount = 1;

		// Sleep until we feel like sleeping would overshoot our target time
		while (timeRemaining > estimatedWait)
		{
			const double waitStartTime = _platform->GetPlatformTimeSeconds();

			_platform->Sleep(1);

			const double waitTime = _platform->GetPlatformTimeSeconds() - waitStartTime;
			timeRemaining -= waitTime;

			const double delta = waitTime - estimatedWait;

			estimatedWait += delta / waitCount;
			waitCount++;
		}

		// Actively wait until our next tick time
		while (_platform->GetPlatformTimeSeconds() < nextTickTime)
		{}
	}
}