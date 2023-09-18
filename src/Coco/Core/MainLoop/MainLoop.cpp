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
		_useAbsoluteTiming(false)
	{}

	MainLoop::~MainLoop()
	{
		_tickListeners.clear();
	}

	void MainLoop::AddListener(TickListener& listener)
	{
		const auto it = std::find(_tickListeners.cbegin(), _tickListeners.cend(), &listener);

		if (it != _tickListeners.cend())
			return;

		_tickListeners.push_back(&listener);
		listener._isRegistered = true;
		_listenersNeedSorting = true;
	}

	void MainLoop::RemoveListener(TickListener& listener)
	{
		const auto it = std::find(_tickListeners.cbegin(), _tickListeners.cend(), &listener);

		if (it == _tickListeners.cend())
			return;

		_tickListeners.erase(it);
		listener._isRegistered = false;
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

	void MainLoop::SetUseAbsoluteTiming(bool useAbsoluteTiming)
	{
		_useAbsoluteTiming = useAbsoluteTiming;
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		EnginePlatform* platform = Engine::Get()->GetPlatform();

		// Initialize the current tick
		_currentTick.TickNumber = 0;
		_currentTick.UnscaledTime = platform->GetRunningTime();
		_currentTick.UnscaledDeltaTime = 0.0;
		_currentTick.Time = _currentTick.UnscaledTime;
		_currentTick.DeltaTime = 0.0;
		_lastTick = _currentTick;

		bool didPerformFullTick = true;
		double preProcessPlatformTime = 0.0;
		double lastTickPlatformTime = 0.0;

		while (_isRunning)
		{
			if (_targetTicksPerSecond > 0)
				WaitForTargetTickTime(lastTickPlatformTime);

			// Save the pre-process time only if we performed a full tick so we can calculate an adjusted delta time
			if (didPerformFullTick)
				preProcessPlatformTime = platform->GetRunningTime();

			platform->ProcessMessages();

			if (_isSuspended)
			{
				didPerformFullTick = false;
				continue;
			}

			if (_listenersNeedSorting)
				SortTickHandlers();

			double currentPlatformTime = Engine::Get()->GetPlatform()->GetRunningTime();
			double timeDelta = currentPlatformTime - lastTickPlatformTime;
			lastTickPlatformTime = currentPlatformTime;

			// Adjust the delta time to be consistent if needed
			if (!_useAbsoluteTiming)
				timeDelta -= currentPlatformTime - preProcessPlatformTime;

			_currentTick.UnscaledDeltaTime = timeDelta;
			_currentTick.UnscaledTime += _currentTick.UnscaledDeltaTime;
			_currentTick.DeltaTime = _currentTick.UnscaledDeltaTime * _timeScale;
			_currentTick.Time += _currentTick.DeltaTime;

			PreTick();

			std::vector<TickListener*> tempListeners(_tickListeners);
			for (auto it = tempListeners.begin(); it != tempListeners.end(); it++)
			{
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

			_lastTick = _currentTick;
			_currentTick.TickNumber++;
			didPerformFullTick = true;
		}
	}

	void MainLoop::Stop()
	{
		_isRunning = false;
	}

	void MainLoop::SortTickHandlers()
	{
		std::sort(_tickListeners.begin(), _tickListeners.end(),
			[](const TickListener* a, const TickListener* b)
			{
				return a->Priority < b->Priority;
			});

		_listenersNeedSorting = false;
	}

	void MainLoop::PreTick()
	{
	}

	void MainLoop::PostTick()
	{
	}

	void MainLoop::WaitForTargetTickTime(double lastTickTime)
	{
		const double nextTickTime = lastTickTime + (1.0 / _targetTicksPerSecond);

		EnginePlatform* platform = Engine::Get()->GetPlatform();

		double timeRemaining = nextTickTime - platform->GetRunningTime();
		double estimatedWait = 0.0;
		uint64 waitCount = 1;

		// Sleep until we feel like sleeping would overshoot our target time
		while (timeRemaining > estimatedWait)
		{
			const double waitStartTime = platform->GetSeconds();
			platform->Sleep(1);
			const double waitTime = platform->GetSeconds() - waitStartTime;

			timeRemaining -= waitTime;

			estimatedWait += (waitTime - estimatedWait) / waitCount;
			waitCount++;
		}

		// Actively wait until our next tick time
		while (platform->GetRunningTime() < nextTickTime)
		{}
	}
}