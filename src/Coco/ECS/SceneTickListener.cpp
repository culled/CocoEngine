#include "ECSpch.h"
#include "SceneTickListener.h"

namespace Coco::ECS
{
	SceneTickListener::SceneTickListener(CallbackFunc callbackFunction, int priority) :
		Priority(priority),
		_callback(callbackFunction)
	{}

	void SceneTickListener::Tick(const TickInfo& tickInfo)
	{
		_callback(tickInfo);
	}
}