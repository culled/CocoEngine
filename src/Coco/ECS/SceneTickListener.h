#pragma once

#include "ECSpch.h"
#include <Coco/Core/MainLoop/TickInfo.h>

namespace Coco::ECS
{
	class SceneTickListener
	{
	public:
		using CallbackFunc = std::function<void(const TickInfo&)>;

		const int Priority;

	public:
		SceneTickListener(CallbackFunc callbackFunction, int priority);

		template<typename ObjectType>
		SceneTickListener(ObjectType* instance, void(ObjectType::* callback)(const TickInfo&), int priority) :
			SceneTickListener(std::bind(callback, instance, std::placeholders::_1), priority)
		{}

		void Tick(const TickInfo& tickInfo);

	private:
		CallbackFunc _callback;
	};
}