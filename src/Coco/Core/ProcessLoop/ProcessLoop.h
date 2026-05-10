//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_PROCESSLOOP_H
#define COCOENGINE_PROCESSLOOP_H
#include "TickListener.h"
#include "../Types/Array.h"

namespace Coco
{
    /// @brief A loop that ticks listeners
    class ProcessLoop
    {
    public:
        ProcessLoop();
        ~ProcessLoop() noexcept;

        /// @brief Adds a TickListener that will be notified of ticks from this loop
        /// @param listener The listener
        void AddTickListener(TickListener& listener);

        /// @brief Removes a TickListener from this loop and stops notifying it of ticks
        /// @param listener The listener
        void RemoveTickListener(const TickListener& listener);

        /// @brief Starts this loop. Will block until the loop is stopped
        void Start();

        /// @brief Signals this loop to stop. It will stop after the current tick finishes
        void Stop();

        /// @brief Sets the time scale of the delta time progression.
        /// 1.0 is normal, 0.5 is half speed, 2.0 is double speed, etc.
        /// @param timeScale The time scale
        void SetTimeScale(double timeScale);

        /// @brief Gets the current time scale
        /// @return The time scale
        double GetTimeScale() const { return _timeScale; }

        /// @brief Sets the tick rate that this loop will try to match, in ticks per second.
        /// The loop will not run faster than this rate, but may run slower depending on the speed of the hardware and complexity of the application.
        /// Setting this to 0 disables the limit and runs the loop as fast as possible
        /// @param targetTicksPerSecond The target tick rate, or 0 to disable the limit
        void SetTargetTickRate(uint16 targetTicksPerSecond);

        /// @brief Gets the tick rate that this loop is trying to run at
        /// @return The target tick rate
        uint16 GetTargetTickRate() const { return _targetTickRate; }

        /// @brief Sets the type of waiting to use when matching a target tick rate.
        /// If this is set to false, then the loop will sleep until right before the next tick, reducing the amount of CPU used.
        /// Greedy waiting essentially causes the loop to constantly loop until the next tick time is reached.
        /// The loop may use greedy waiting after sleeping to ensure a consistent tick rate, but may also overshoot if the loop isn't woken up soon enough by the kernel
        /// @param useGreedyWait The greedy wait state
        void SetUseGreedyWait(bool useGreedyWait);

        /// @brief Gets the type of waiting being used by this loop when matching a target tick rate
        /// @return The greedy wait state
        bool IsUsingGreedyWait() const { return _useGreedyWait; }

        /// @brief Sets the amount of caution to use when calculating the estimated sleep time to match a target tick rate.
        /// This essentially multiplies the average time spent sleeping to determine when to use greedy waiting to achieve an accurate tick rate.
        /// Too low of a value may cause the loop to oversleep, and too high may cause unnecessary greedy waiting.
        /// @param caution The caution value. Values should be 1.0 or higher
        void SetEstimatedSleepCaution(double caution);

        /// @brief Gets the amount of caution to use when calculating the estimated sleep time to match a target tick rate
        /// @return The caution value
        double GetEstimatedSleepCaution() const { return _estimatedWaitCaution; }

        /// @brief Gets the current tick info
        /// @return The current tick info
        const TickInfo& GetCurrentTick() const { return _currentTickInfo; }

        /// @brief Gets the previous tick info
        /// @return The last tick info
        const TickInfo& GetLastTick() const { return _lastTickInfo; }

    private:
        static const TimeSpan _maxDeltaTime;

        bool _isRunning;
        uint16 _targetTickRate;
        double _timeScale;
        TickInfo _currentTickInfo;
        TickInfo _lastTickInfo;
        bool _useGreedyWait;
        double _estimatedWaitCaution;
        TimeSpan _averageSleepTime;
        Array<TickListener*> _tickListeners;
        int64 _currentTickListenerIndex;
        bool _tickListenersNeedSorting;

        /// @brief Blocks until the next tick should fire to match the target tick rate
        void WaitForTargetTickRate();

        /// @brief Sorts the TickListeners by their order
        void SortTickListeners();

        /// @brief Processes and dispatches a tick to all listening TickListeners
        void ProcessTick();
    };
} // Coco

#endif //COCOENGINE_PROCESSLOOP_H