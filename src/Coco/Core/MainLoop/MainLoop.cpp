#include "Corepch.h"
#include "MainLoop.h"

#include "../Engine.h"

namespace Coco
{
	MainLoop::MainLoop() :
		_tickListeners{},
		_isRunning(false),
		_targetTicksPerSecond(0),
		_isSuspended(false),
		_listenersNeedSorting(false),
		_currentTick{},
		_lastTick{},
		_timeScale(1.0),
		_lastAverageSleepTime(0.0)
	{}

	MainLoop::~MainLoop()
	{
		_tickListeners.clear();
	}

	void MainLoop::AddListener(Ref<TickListener> listener)
	{
		const auto it = std::find_if(_tickListeners.cbegin(), _tickListeners.cend(), [listener](const Ref<TickListener>& other)
			{
				return other.Get() == listener.Get();
			});

		if (it != _tickListeners.cend())
			return;

		_tickListeners.push_back(listener);
		listener->_isRegistered = true;
		_listenersNeedSorting = true;
	}

	void MainLoop::RemoveListener(Ref<TickListener> listener)
	{
		const auto it = std::find_if(_tickListeners.cbegin(), _tickListeners.cend(), [listener](const Ref<TickListener>& other)
			{
				return other.Get() == listener.Get();
			});

		if (it == _tickListeners.cend())
			return;

		_tickListeners.erase(it);
		listener->_isRegistered = false;
	}

	void MainLoop::SetTargetTicksPerSecond(uint32 ticksPerSecond)
	{
		_targetTicksPerSecond = ticksPerSecond;
	}

	void MainLoop::SetSuspended(bool suspended)
	{
		_isSuspended = suspended;
	}

	void MainLoop::SetTimeScale(double timeScale)
	{
		_timeScale = timeScale;
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		EnginePlatform& platform = Engine::Get()->GetPlatform();
		double currentPlatformTime = platform.GetSeconds();
		double lastTickPlatformTime = currentPlatformTime;

		// Initialize the current tick
		_currentTick.TickNumber = 0;
		_currentTick.UnscaledTime = platform.GetRunningTime();
		_currentTick.UnscaledDeltaTime = 0.0;
		_currentTick.Time = _currentTick.UnscaledTime;
		_currentTick.DeltaTime = 0.0;
		_lastTick = _currentTick;

		bool didPerformFullTick = true;
		//double preProcessPlatformTime = platform.GetSeconds();

		while (_isRunning)
		{
			// Save the pre-process time only if we performed a full tick so we can calculate an adjusted delta time
			//if (didPerformFullTick)
			//	preProcessPlatformTime = platform.GetSeconds();

			// TODO: non-absolute timing with long message loops
			currentPlatformTime = platform.GetSeconds();
			double timeDelta = currentPlatformTime - lastTickPlatformTime;

			_currentTick.UnscaledDeltaTime = timeDelta;
			_currentTick.UnscaledTime += _currentTick.UnscaledDeltaTime;
			_currentTick.DeltaTime = _currentTick.UnscaledDeltaTime * _timeScale;
			_currentTick.Time += _currentTick.DeltaTime;

			platform.ProcessMessages();

			if (!_isSuspended)
			{
				PreTick();

				std::vector<Ref<TickListener>> tempListeners(_tickListeners);
				for (auto it = tempListeners.begin(); it != tempListeners.end(); it++)
				{
					if (!it->IsValid())
						continue;

					try
					{
						(*it)->Invoke(_currentTick);
					}
					catch (const std::exception& ex)
					{
						CocoError("Error ticking: {}", ex.what());
					}
				}

				PostTick();

				didPerformFullTick = true;
			}
			else
			{
				didPerformFullTick = false;
			}

			_lastTick = _currentTick;
			_currentTick.TickNumber++;
			lastTickPlatformTime = currentPlatformTime;

			if (_targetTicksPerSecond > 0)
				WaitForTargetTickTime(currentPlatformTime);
		}
	}

	void MainLoop::Stop()
	{
		_isRunning = false;
	}

	void MainLoop::SortTickHandlers()
	{
		std::sort(_tickListeners.begin(), _tickListeners.end(),
			[](const Ref<TickListener>& a, const Ref<TickListener>& b)
			{
				if (!a.IsValid())
					return false;

				if (!b.IsValid())
					return true;

				return a->Priority < b->Priority;
			});

		_listenersNeedSorting = false;
	}

	void MainLoop::PreTick()
	{
		if (_listenersNeedSorting)
			SortTickHandlers();
	}

	void MainLoop::PostTick()
	{
		auto it = _tickListeners.begin();

		while (it != _tickListeners.end())
		{
			if (!it->IsValid())
			{
				it = _tickListeners.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void MainLoop::WaitForTargetTickTime(double lastTickTime)
	{
		EnginePlatform& platform = Engine::Get()->GetPlatform();

		const double nextTickTime = lastTickTime + (1.0 / _targetTicksPerSecond);

		double timeRemaining = nextTickTime - platform.GetSeconds();

		if (timeRemaining < 0.0)
			return;

		double estimatedWait = _lastAverageSleepTime;
		uint64 waitCount = 1;

		// Sleep until we feel like sleeping would overshoot our target time
		while (timeRemaining > estimatedWait)
		{
			const double waitStartTime = platform.GetSeconds();
			platform.Sleep(1);
			const double waitTime = platform.GetSeconds() - waitStartTime;
		
			timeRemaining -= waitTime;
		
			estimatedWait += (waitTime - estimatedWait) / waitCount;
			waitCount++;
		}

		// Make the average wait slowly decrease to prevent spikes from disabling it completely
		_lastAverageSleepTime = estimatedWait * 0.99;

		// Actively wait until our next tick time
		while (platform.GetSeconds() < nextTickTime)
		{}
	}
}