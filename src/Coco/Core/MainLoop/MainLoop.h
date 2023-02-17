#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	namespace Platform
	{
		class EnginePlatform;
	}

	class COCOAPI MainLoop
	{
	private:
		Platform::EnginePlatform* _platform;
		bool _isRunning;
		bool _isSuspended;

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
	};
}

