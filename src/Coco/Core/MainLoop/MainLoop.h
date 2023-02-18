#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/DateTime.h>
#include <Coco/Core/Types/TimeSpan.h>

namespace Coco
{
	namespace Platform
	{
		class EnginePlatform;
	}

	class MainLoopTickListener;

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

		void AddTickListener(Ref<MainLoopTickListener> tickListener);
		void RemoveTickListener(const Ref<MainLoopTickListener>& tickListener);

		void SetTimeScale(double timeScale) { _timeScale = timeScale; }
		double GetTimeScale() const { return _timeScale; }

	private:
		static bool CompareTickListeners(const Ref<MainLoopTickListener>& a, const Ref<MainLoopTickListener>& b);

		void SortTickListeners();
	};
}

