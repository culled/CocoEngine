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
	};
}

