#include "MainLoop.h"

#include <Coco/Core/Platform/EnginePlatform.h>

namespace Coco
{
	MainLoop::MainLoop(Platform::EnginePlatform* platform) :
		_platform(platform), _isRunning(false), _isSuspended(false)
	{
	}

	void MainLoop::Run()
	{
		_isRunning = true;

		while (_isRunning)
		{
			_platform->HandlePlatformMessages();

			// Only process messages if suspended
			if (_isSuspended)
			{
				// TODO: sleep
				continue;
			}
		}
	}

	void MainLoop::Stop()
	{
		_isRunning = false;
	}

	void MainLoop::SetIsSuspended(bool isSuspended)
	{
		_isSuspended = isSuspended;
	}
}