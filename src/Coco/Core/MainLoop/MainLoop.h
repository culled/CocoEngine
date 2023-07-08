#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>

#include "MainLoopTickListener.h"

namespace Coco
{
	namespace Platform
	{
		class IEnginePlatform;
	}

	/// @brief The loop that runs for the lifetime of the application and dispatches ticks to MainLoopTickListeners
	class COCOAPI MainLoop
	{
	private:
		static constexpr uint s_suspendSleepPeriodMs = 1;

		bool _isRunning = false;
		bool _isSuspended = false;
		bool _useAbsoluteTiming = false;

		List<ManagedRef<MainLoopTickListener>> _tickListeners;
		bool _tickListenersNeedSorting = false;

		bool _isPerfomingTick = false;
		List<Ref<MainLoopTickListener>> _tickListenersToRemove;

		double _currentTickTime = 0.0;
		double _lastTickTime = 0.0;

		double _timeScale = 1.0;

		double _currentUnscaledRunningTime = 0.0;
		double _currentUnscaledDeltaTime = 0.0;
		double _currentRunningTime = 0.0;
		double _currentDeltaTime = 0.0;

		int _targetTickRate = 0;

		uint64_t _tickCount = 0;

	public:
		MainLoop() = default;

		/// @brief Runs this loop and blocks until it has stopped
		void Run();

		/// @brief Stops this loop from running after the current tick finishes
		void Stop() noexcept;

		/// @brief Sets the suspended state of the loop.
		/// Suspended means the application will only handle platform messages, essentially pausing everything else
		/// @param isSuspended The suspended state
		void SetIsSuspended(bool isSuspended) noexcept;

		/// @brief Gets the suspended state of the loop
		/// @return True if the loop is suspended
		constexpr bool GetIsSuspended() const noexcept { return _isSuspended; }

		/// @brief Sets if this loop should use absolute timing
		/// If false, the loop will attempt to maintain a consistent delta time across suspensions/platform message loops, 
		/// at the expense of running time becoming out of sync with the actual time passing. The default is false
		/// @param useAbsoluteTiming If true, this loop will use absolute timing
		constexpr void SetUseAbsoluteTiming(bool useAbsoluteTiming) noexcept { _useAbsoluteTiming = useAbsoluteTiming; }

		/// @brief Gets if this loop is using absolute timing
		/// @return If true, this loop is using absolute timing
		constexpr bool GetUseAbsoluteTiming() const noexcept { return _useAbsoluteTiming; }

		/// @brief Creates a tick listener on the main loop
		/// @param args Arguments to pass to the listener's constructor
		template<typename ObjectType>
		Ref<MainLoopTickListener> CreateTickListener(ObjectType* object, void(ObjectType::* handlerFunction)(double), int priority)
		{
			_tickListeners.Add(CreateManagedRef<MainLoopTickListener>(object, handlerFunction, priority));
			_tickListenersNeedSorting = true;

			return _tickListeners.Last();
		}

		/// @brief Removes a tick listener from the main loop
		/// @param tickListener The listener to remove
		void RemoveTickListener(const Ref<MainLoopTickListener>& tickListener) noexcept;

		/// @brief Sets the time scale of the main loop
		/// @param timeScale The relative scale that time passes
		constexpr void SetTimeScale(double timeScale) noexcept { _timeScale = timeScale; }

		/// @brief Gets the time scale of the main loop
		/// @return The relative scale that time passes
		constexpr double GetTimeScale() const noexcept { return _timeScale; }

		/// @brief Sets the target ticks per seconds for the loop to tick at
		/// @param targetTickRate The target ticks per second to run the engine at. Values equal to or less than 0 make the engine run as fast as possible
		constexpr void SetTargetTickRate(int targetTickRate) noexcept { _targetTickRate = targetTickRate; }

		/// @brief Gets the target ticks per seconds for the loop to tick at
		/// @return The target ticks per seconds for the loop to tick at
		constexpr int GetTargetTickRate() const noexcept { return _targetTickRate; }

		/// @brief Gets the cumulative time the loop has been running, regardless of the time scale
		/// @return The unscaled amount of time the loop has been running
		constexpr double GetUnscaledRunningTime() const noexcept { return _currentUnscaledRunningTime; }

		/// @brief Gets the current tick delta time, regardless of the time scale
		/// @return The current tick unscaled delta time, in seconds
		constexpr double GetUnscaledDeltaTime() const noexcept { return _currentUnscaledDeltaTime; }

		/// @brief Gets the cumulative time the loop has been running, taking into account the time scale
		/// @return The scaled amount of time the loop has been running
		constexpr double GetRunningTime() const noexcept { return _currentRunningTime; }

		/// @brief Gets the current tick delta time, taking into account the time scale
		/// @return The current tick scaled delta time, in seconds
		constexpr double GetDeltaTime() const noexcept { return _currentDeltaTime; }

		/// @brief Gets the number of ticks that have occurred
		/// @return The number of ticks that have occurred
		constexpr uint64_t GetTickCount() const noexcept { return _tickCount; }

	private:
		/// @brief Compares two tick listeners
		/// @param a The first listener
		/// @param b The second listener
		/// @return True if listener A should be placed before listener B
		static bool CompareTickListeners(const MainLoopTickListener* a, const MainLoopTickListener* b) noexcept;

		/// @brief Called immediately before a tick runs
		/// @param preTickTime The time that this tick would've executed if it wasn't held up by suspension/processing loops
		void PreTick(double preTickTime);

		/// @brief Called immediately after a tick finishes 
		void PostTick();

		/// @brief Sorts the list of tick listeners
		void SortTickListeners() noexcept;

		/// @brief Blocks the thread until the next tick time
		void WaitForNextTick();
	};
}

