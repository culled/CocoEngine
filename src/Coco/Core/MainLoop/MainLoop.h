#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/DateTime.h>
#include <Coco/Core/Types/TimeSpan.h>

#include "MainLoopTickListener.h"

namespace Coco
{
	namespace Platform
	{
		class EnginePlatform;
	}

	/// <summary>
	/// The loop that runs for the lifetime of the application and dispatches ticks
	/// </summary>
	class COCOAPI MainLoop
	{
	private:
		Platform::EnginePlatform* _platform;

		bool _isRunning;
		bool _isSuspended;
		bool _useAbsoluteTiming;

		List<Ref<MainLoopTickListener>> _tickListeners;
		bool _tickListenersNeedSorting;

		DateTime _currentTickTime;
		DateTime _lastTickTime;
		TimeSpan _deltaTime;

		double _timeScale;

		double _currentUnscaledRunningTime;
		double _currentUnscaledDeltaTime;
		double _currentRunningTime;
		double _currentDeltaTime;

		int _targetTickRate;

	public:
		MainLoop(Platform::EnginePlatform* platform);

		/// <summary>
		/// Runs this loop and blocks until it has stopped
		/// </summary>
		void Run();

		/// <summary>
		/// Stops this loop from running
		/// </summary>
		void Stop();

		/// <summary>
		/// Sets the suspended state of the loop.
		/// Suspended means the application will only handle platform messages, essentially pausing everything else
		/// </summary>
		/// <param name="isSuspended">The suspended state</param>
		void SetIsSuspended(bool isSuspended);

		/// <summary>
		/// Gets the suspended state of the loop
		/// </summary>
		/// <returns>True if the loop is suspended</returns>
		bool GetIsSuspended() const { return _isSuspended; }

		/// <summary>
		/// Sets if this loop should use absolute timing
		/// If false, the loop will attempt to maintain a consistent delta time across suspensions/platform message loops, 
		/// at the expense of running time becoming out of sync with the actual time passing. The default is false
		/// </summary>
		/// <param name="useAbsoluteTiming">If true, this loop will use absolute timing</param>
		void SetUseAbsoluteTiming(bool useAbsoluteTiming) { _useAbsoluteTiming = useAbsoluteTiming; }

		/// <summary>
		/// Gets if this loop is using absolute timing
		/// </summary>
		/// <returns>If true, this loop is using absolute timing</returns>
		bool GetUseAbsoluteTiming() const { return _useAbsoluteTiming; }

		/// <summary>
		/// Adds a tick listener to the main loop
		/// </summary>
		/// <param name="tickListener">The listener to add</param>
		void AddTickListener(Ref<MainLoopTickListener> tickListener);

		/// <summary>
		/// Removes a tick listener from the main loop
		/// </summary>
		/// <param name="tickListener">The listener to remove</param>
		void RemoveTickListener(const Ref<MainLoopTickListener>& tickListener);

		/// <summary>
		/// Sets the time scale of the main loop
		/// </summary>
		/// <param name="timeScale">The relative scale that time passes</param>
		void SetTimeScale(double timeScale) { _timeScale = timeScale; }

		/// <summary>
		/// Gets the time scale of the main loop
		/// </summary>
		/// <returns>The relative scale that time passes</returns>
		double GetTimeScale() const { return _timeScale; }

		/// <summary>
		/// Sets the target ticks per seconds for the loop to tick at
		/// </summary>
		/// <param name="targetTickRate">The target ticks per second to run the engine at. Values equal to or less than 0 make the engine run as fast as possible</param>
		void SetTargetTickRate(int targetTickRate) { _targetTickRate = targetTickRate; }

		/// <summary>
		/// Gets the target ticks per seconds for the loop to tick at
		/// </summary>
		/// <returns>The target ticks per seconds for the loop to tick at</returns>
		int GetTargetTickRate() const { return _targetTickRate; }

		/// <summary>
		/// Gets the cumulative time the loop has been running, regardless of the time scale
		/// </summary>
		/// <returns>The unscaled amount of time the loop has been running</returns>
		double GetUnscaledRunningTime() const { return _currentUnscaledRunningTime; }

		/// <summary>
		/// Gets the current tick delta time, regardless of the time scale
		/// </summary>
		/// <returns>The current tick unscaled delta time, in seconds</returns>
		double GetUnscaledDeltaTime() const { return _currentUnscaledDeltaTime; }

		/// <summary>
		/// Gets the cumulative time the loop has been running, taking into account the time scale
		/// </summary>
		/// <returns>The scaled amount of time the loop has been running</returns>
		double GetRunningTime() const { return _currentRunningTime; }

		/// <summary>
		/// Gets the current tick delta time, taking into account the time scale
		/// </summary>
		/// <returns>The current tick scaled delta time, in seconds</returns>
		double GetDeltaTime() const { return _currentDeltaTime; }

	private:
		/// <summary>
		/// Compares two tick listeners
		/// </summary>
		/// <param name="a">The first listener</param>
		/// <param name="b">The second listener</param>
		/// <returns>True if listener A should be placed before listener B</returns>
		static bool CompareTickListeners(const Ref<MainLoopTickListener>& a, const Ref<MainLoopTickListener>& b);

		/// <summary>
		/// Sorts the list of tick listeners
		/// </summary>
		void SortTickListeners();

		/// <summary>
		/// Blocks the thread until the next tick time
		/// </summary>
		void WaitForNextTick();
	};
}

