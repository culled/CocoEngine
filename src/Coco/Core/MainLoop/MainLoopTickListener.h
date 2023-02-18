#pragma once

#include <Coco/Core/Core.h>

#include <functional>

namespace Coco
{
	class COCOAPI MainLoopTickListener
	{
	public:
		using TickHandler = std::function<void(double)>;

		const int Priority;

	private:
		TickHandler _handler;
		bool _isEnabled;

	public:
		template<typename ObjectType>
		MainLoopTickListener(ObjectType* object, void(ObjectType::* handlerFunction)(double), int priority) : 
			MainLoopTickListener(std::bind(handlerFunction, object, std::placeholders::_1), priority)
		{}

		MainLoopTickListener(TickHandler tickHandler, int priority);
		virtual ~MainLoopTickListener() = default;

		void SetIsEnabled(bool isEnabled) { _isEnabled = isEnabled; }
		bool GetIsEnabled() const { return _isEnabled; }

	private:
		friend class MainLoop;

		void Tick(double deltaTime);
	};
}