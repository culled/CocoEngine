#include "MainLoop.h"

#include <Coco/Core/Platform/IEnginePlatform.h>

#include "MainLoopTickListener.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	void MainLoop::Run()
	{
		Platform::IEnginePlatform* platform = Engine::Get()->GetPlatform();

		_isRunning = true;

		// Set the current time to the current time
		_currentTickTime = platform->GetRunningTimeSeconds();
		_lastTickTime = _currentTickTime;

		double preTickTime = _currentTickTime;
		bool didTick = true;

		while (_isRunning)
		{

			// Only record the time once each loop
			// Without this, we wouldn't account for time spent suspended as the loop only partially runs while suspended
			if (didTick)
			{
				preTickTime = platform->GetRunningTimeSeconds();
				didTick = false;
			}

			platform->HandlePlatformMessages();

			// Only process messages if suspended
			if (_isSuspended)
			{
				platform->Sleep(s_suspendSleepPeriodMs);
				continue;
			}

			PreTick(preTickTime);

			_isPerfomingTick = true;

			for (ManagedRef<MainLoopTickListener>& listener : _tickListeners)
			{
				listener->Tick(_currentDeltaTime);
			}

			_isPerfomingTick = false;

			PostTick();

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

	void MainLoop::RemoveTickListener(const Ref<MainLoopTickListener>& tickListener) noexcept
	{
		// Queue removals if listeners are removed during a tick
		if (_isPerfomingTick)
		{
			_tickListenersToRemove.Add(tickListener);
			return;
		}

		try
		{
			_tickListeners.RemoveAll([tickListener](const auto& other) { return other.Get() == tickListener.Get(); });
		}
		catch(...)
		{ }
	}

	bool MainLoop::CompareTickListeners(const MainLoopTickListener* a, const MainLoopTickListener* b) noexcept
	{
		// If true, a gets placed before b
		return a->Priority < b->Priority;
	}

	void MainLoop::PreTick(double preTickTime)
	{
		if (_tickListenersNeedSorting)
			SortTickListeners();

		_currentTickTime = Engine::Get()->GetPlatform()->GetRunningTimeSeconds();

		// Calculate time since the last tick
		_currentUnscaledDeltaTime = _currentTickTime - _lastTickTime;

		// Optionally remove the time spent processing messages/suspended, preventing massive delta times while caught in a message process loop
		if (!_useAbsoluteTiming)
			_currentUnscaledDeltaTime -= _currentTickTime - preTickTime;

		_currentUnscaledRunningTime += _currentUnscaledDeltaTime;

		// Scaled delta and running times
		_currentDeltaTime = _currentUnscaledDeltaTime * _timeScale;
		_currentRunningTime += _currentDeltaTime;
	}

	void MainLoop::PostTick()
	{
		// Safely remove any queued listeners
		for (const auto& listener : _tickListenersToRemove)
			RemoveTickListener(listener);

		_tickListenersToRemove.Clear();
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

		Platform::IEnginePlatform* platform = Engine::Get()->GetPlatform();

		double timeRemaining = nextTickTime - platform->GetRunningTimeSeconds();
		double estimatedWait = 0;
		int waitCount = 1;

		// Sleep until we feel like sleeping would overshoot our target time
		while (timeRemaining > estimatedWait)
		{
			const double waitStartTime = platform->GetRunningTimeSeconds();

			platform->Sleep(1);

			const double waitTime = platform->GetRunningTimeSeconds() - waitStartTime;
			timeRemaining -= waitTime;

			const double delta = waitTime - estimatedWait;

			estimatedWait += delta / waitCount;
			waitCount++;
		}

		// Actively wait until our next tick time
		while (platform->GetRunningTimeSeconds() < nextTickTime)
		{}
	}
}