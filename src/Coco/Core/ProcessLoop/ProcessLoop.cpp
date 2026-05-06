//
// Created by cullen on 2/25/26.
//

#include "ProcessLoop.h"

#include "Coco/Core/Engine.h"
#include "Coco/Core/Types/Sorting/QSorter.h"

namespace Coco
{
    const TimeSpan ProcessLoop::_maxDeltaTime = TimeSpan::FromSeconds(1.0);

    ProcessLoop::ProcessLoop() :
        _isRunning(false),
        _targetTickRate(0),
        _timeScale(1.0),
        _currentTickInfo(),
        _lastTickInfo(),
        _useGreedyWait(false),
        _estimatedWaitCaution(1.5),
        _averageSleepTime(),
        _tickListeners(nullptr, 10),
        _currentTickListenerIndex(0),
        _tickListenersNeedSorting(false)
    {}

    ProcessLoop::~ProcessLoop() noexcept
    {
        try
        {
            while (!_tickListeners.IsEmpty())
                _tickListeners.Back()->StopListening();
        }
        catch (...)
        {
            COCO_ASSERT(false, "Error stopping remaining TickListeners");
        }
    }

    void ProcessLoop::AddTickListener(TickListener& listener)
    {
        _tickListeners.Append(&listener);
        _tickListenersNeedSorting = true;
    }

    void ProcessLoop::RemoveTickListener(const TickListener& listener)
    {
        int64 index = _tickListeners.Find([listener = &listener](const TickListener* other) { return other == listener; });
        if (index == -1)
            return;

        if (index <= _currentTickListenerIndex)
            --_currentTickListenerIndex;

        _tickListeners.RemoveAt(index);
    }

    void ProcessLoop::Start()
    {
        _isRunning = true;

        while (_isRunning)
        {
            if (_tickListenersNeedSorting)
                SortTickListeners();

            ProcessTick();

            if (_targetTickRate > 0)
                WaitForTargetTickRate();
        }
    }

    void ProcessLoop::Stop()
    {
        _isRunning = false;
    }

    void ProcessLoop::SetTimeScale(double timeScale)
    {
        _timeScale = timeScale;
    }

    void ProcessLoop::SetTargetTickRate(uint16 targetTicksPerSecond)
    {
        _targetTickRate = targetTicksPerSecond;
    }

    void ProcessLoop::SetUseGreedyWait(bool useGreedyWait)
    {
        _useGreedyWait = useGreedyWait;
    }

    void ProcessLoop::SetEstimatedSleepCaution(double caution)
    {
        COCO_ASSERT(caution > 0.0, "Caution must be greater than 0");

        _estimatedWaitCaution = caution;
    }

    void ProcessLoop::WaitForTargetTickRate()
    {
        auto platform = Engine::Get()->GetPlatform();

        const TimeSpan nextTickTime = _currentTickInfo.UnscaledTime + TimeSpan::FromSeconds(1.0 / _targetTickRate);
        TimeSpan timeRemaining = nextTickTime - platform->GetRunningTime();

        if (timeRemaining <= 0)
            return;

        if (!_useGreedyWait)
        {
            TimeSpan estimatedWait = Math::Max(_averageSleepTime, TimeSpan::FromMilliseconds(1));
            uint64 waitCount = 1;
            const TimeSpan sleepPeriod = TimeSpan::FromMilliseconds(1);

            // Sleep until we feel like sleeping would overshoot our target time
            while (timeRemaining > estimatedWait * _estimatedWaitCaution)
            {
                const TimeSpan waitStartTime = platform->GetCurrentTime();
                platform->Sleep(sleepPeriod);
                const TimeSpan elapsedTime = platform->GetCurrentTime() - waitStartTime;

                timeRemaining -= elapsedTime;

                estimatedWait += (elapsedTime - estimatedWait) / static_cast<double>(waitCount);
                ++waitCount;
            }

            // Make the average wait slowly decrease to prevent spikes from disabling it completely
            _averageSleepTime = estimatedWait * 0.99;
        }

        // Actively wait until our next tick time
        while (platform->GetRunningTime() < nextTickTime)
        {}
    }

    void ProcessLoop::SortTickListeners()
    {
        QSorter<TickListener*> sorter([](const TickListener* a, const TickListener* b)
            {
                return a->GetOrder() < b->GetOrder();
            });

        sorter.Sort(_tickListeners);

        _tickListenersNeedSorting = false;
    }

    void ProcessLoop::ProcessTick()
    {
        _lastTickInfo = _currentTickInfo;

        _currentTickInfo.UnscaledTime = Engine::Get()->GetPlatform()->GetRunningTime();

        if (_currentTickInfo.TickNumber == 0)
        {
            _currentTickInfo.UnscaledDeltaTime = TimeSpan::FromSeconds(1.0 / 60.0);
        }
        else
        {
            _currentTickInfo.UnscaledDeltaTime = _currentTickInfo.UnscaledTime - _lastTickInfo.UnscaledTime;
        }

        _currentTickInfo.DeltaTime = Math::Min(_currentTickInfo.UnscaledDeltaTime, _maxDeltaTime) * _timeScale;
        _currentTickInfo.Time += _currentTickInfo.DeltaTime;
        ++_currentTickInfo.TickNumber;

        for (_currentTickListenerIndex = 0; _currentTickListenerIndex < static_cast<int64>(_tickListeners.GetCount()); _currentTickListenerIndex++)
        {
            _tickListeners[_currentTickListenerIndex]->DispatchTick(_currentTickInfo);
        }
    }
} // Coco
