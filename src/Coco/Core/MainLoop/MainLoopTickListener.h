#pragma once

#include <Coco/Core/Core.h>

#include <functional>

namespace Coco
{
	/// <summary>
	/// A listener that can be added to the main loop to call a tick function
	/// </summary>
	class COCOAPI MainLoopTickListener
	{
	public:
		using TickHandler = std::function<void(double)>;

		/// <summary>
		/// The priority for this listener.
		/// Lower priorities get ticked before higher priorities
		/// </summary>
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

		/// <summary>
		/// Sets this listeners as enabled
		/// </summary>
		/// <param name="isEnabled">If true, this listener (and its callback) will receive ticks</param>
		void SetIsEnabled(bool isEnabled) { _isEnabled = isEnabled; }

		/// <summary>
		/// Gets if this listener is enabled
		/// </summary>
		/// <returns>True if this listener is responding to ticks
		bool GetIsEnabled() const { return _isEnabled; }

	private:
		friend class MainLoop;

		void Tick(double deltaTime);
	};
}