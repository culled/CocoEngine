#pragma once

#include "../Corepch.h"
#include "../Types/Singleton.h"
#include "../Types/Refs.h"
#include "TickInfo.h"
#include "TickListener.h"
#include "../Types/TickSystem.h"

namespace Coco
{
	/// @brief A loop that ticks listeners
	class MainLoop : 
		public Singleton<MainLoop>,
		public TickSystem<TickListener, const TickInfo&>
	{
	public:
		MainLoop();

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

		/// @brief Sets the time of the current tick
		/// @param seconds The current time, in seconds
		void SetCurrentTickTime(double seconds);

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
		bool _isRunning;
		uint32 _targetTicksPerSecond;
		bool _isSuspended;
		TickInfo _currentTick;
		TickInfo _lastTick;
		double _timeScale;
		double _lastAverageSleepTime;

	private:
		/// @brief Compares two tick listeners for sorting
		/// @param a The first listener
		/// @param b The second listener
		/// @return True if a should be sorted before b
		static bool CompareTickListeners(const Ref<TickListener>& a, const Ref<TickListener>& b);

		/// @brief Dispatches a tick to a listener
		/// @param listener The listener
		/// @param tickInfo The tick info
		static void PerformTick(Ref<TickListener>& listener, const TickInfo& tickInfo);

		/// @brief Handler for errors during the a tick
		/// @param ex The exception
		/// @return True if the error was handled
		static bool HandleTickError(const std::exception& ex);

		/// @brief Waits for the next tick based on the target tick rate
		/// @param lastTickTime The platform-time of the last tick
		void WaitForTargetTickTime(double lastTickTime);
	};
}