#include "MainLoop.h"

#include <Coco/Core/Platform/EnginePlatform.h>

#include "MainLoopTickListener.h"

namespace Coco
{
	MainLoop::MainLoop(Platform::EnginePlatform* platform) :
		_platform(platform),
		_isRunning(false),
		_isSuspended(false),
		_tickListenersNeedSorting(false),
		_timeScale(1.0),
		_currentUnscaledDeltaTime(0),
		_currentUnscaledRunningTime(0),
		_currentDeltaTime(0),
		_currentRunningTime(0)
	{
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		_currentTickTime = _platform->GetPlatformLocalTime();
		_lastTickTime = _currentTickTime;

		while (_isRunning)
		{
			_platform->HandlePlatformMessages();

			// Only process messages if suspended
			if (_isSuspended)
			{
				// TODO: sleep
				continue;
			}

			if (_tickListenersNeedSorting)
				SortTickListeners();

			List<Ref<MainLoopTickListener>> listenersCopy = _tickListeners;

			_currentTickTime = _platform->GetPlatformLocalTime();
			_deltaTime = _currentTickTime - _lastTickTime;

			_currentUnscaledDeltaTime = _deltaTime.GetTotalSeconds();
			_currentUnscaledRunningTime += _currentUnscaledDeltaTime;

			_currentDeltaTime = _currentUnscaledDeltaTime * _timeScale;
			_currentRunningTime += _currentDeltaTime;

			for (const Ref<MainLoopTickListener>& listener : listenersCopy)
			{
				listener->Tick(_currentDeltaTime);
			}

			_lastTickTime = _currentTickTime;
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
}