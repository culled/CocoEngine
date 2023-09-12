#pragma once

#include "../Corepch.h"
#include "TickInfo.h"
#include "TickListener.h"

namespace Coco
{
	/// @brief A loop that ticks listeners
	class MainLoop
	{
	private:
		std::vector<TickListener*> _tickListeners;
		bool _isRunning;
		uint32 _targetTicksPerSecond;
		bool _isSuspended;
		bool _listenersNeedSorting;
		TickInfo _currentTick;
		TickInfo _lastTick;
		double _timeScale;
		bool _useAbsoluteTiming;

	public:
		MainLoop();
		~MainLoop();

		/// @brief Adds a listener to this loop
		/// @param listener The listener
		void AddListener(TickListener& listener);

		/// @brief Removes a listener from this loop
		/// @param listener The listener
		void RemoveListener(TickListener& listener);

		/// @brief Sets the target amount of ticks per second that the loop will run at
		/// @param ticksPerSecond The target number of ticks per second. Set to 0 to run as fast as possible
		void SetTargetTicksPerSecond(uint32 ticksPerSecond);

		/// @brief Gets the target ticks per second that this loop is trying to run at
		/// @return The target ticks per second
		uint32 GetTargetTicksPerSecond() const { return _targetTicksPerSecond; }

		/// @brief Sets the suspended state of this loop.
		/// Suspended means no ticks will be processed, but messages from the platform will still be processed
		/// @param suspended If true, the loop will be suspended
		void SetSuspended(bool suspended);

		/// @brief Gets the suspended state of this loop
		/// @return The suspended state
		bool GetIsSuspended() const { return _isSuspended; }

		/// @brief Sets the time scale of the loop
		/// @param timeScale The time scale
		void SetTimeScale(double timeScale);

		/// @brief Gets the time scale
		/// @return The time scale
		double GetTimeScale() const { return _timeScale; }

		/// @brief Sets this loop to use absolute timing.
		/// NOTE: this may introduce huge delta times if the application spends lots of time in a platform message processing loop (e.x. dragging a window)
		/// @param useAbsoluteTiming If true, this loop's timing will more closely match realtime
		void SetUseAbsoluteTiming(bool useAbsoluteTiming);

		/// @brief Gets if this loop is using absolute timing
		/// @return If this loop is using absolute timing
		bool GetUseAbsoluteTiming() const { return _useAbsoluteTiming; }

		/// @brief Gets the current tick info
		/// @return The current tick info
		const TickInfo& GetCurrentTick() const { return _currentTick; }

		/// @brief Gets the last tick info
		/// @return The last tick info
		const TickInfo& GetLastTickTime() const { return _lastTick; }

		/// @brief Runs this loop.
		/// NOTE: will block until the loop is stopped
		void Run();

		/// @brief Marks this loop to stop after the current tick finishes
		void Stop();

	private:
		/// @brief Sorts the list of tick handlers based on their priorities
		void SortTickHandlers();

		/// @brief Called right before handlers are ticked
		void PreTick();

		/// @brief Called right after handlers are ticked
		void PostTick();

		/// @brief Waits for the next tick based on the target tick rate
		void WaitForTargetTickTime();
	};
}