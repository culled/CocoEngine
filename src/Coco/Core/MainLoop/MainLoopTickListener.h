#pragma once

#include <Coco/Core/Core.h>

#include <functional>

namespace Coco
{
	/// @brief A listener of ticks from a MainLoop that performs a callback every tick
	class COCOAPI MainLoopTickListener
	{
	public:
		/// @brief Function signature for a tick callback
		using TickHandler = std::function<void(double)>;

		/// @brief The priority for this listener. Lower priorities get ticked before higher priorities
		const int Priority;

	private:
		TickHandler _handler;
		bool _isEnabled;

	public:
		MainLoopTickListener(TickHandler tickHandler, int priority);

		template<typename ObjectType>
		MainLoopTickListener(ObjectType* object, void(ObjectType::* handlerFunction)(double), int priority) :
			MainLoopTickListener(std::bind(handlerFunction, object, std::placeholders::_1), priority)
		{}

		virtual ~MainLoopTickListener() = default;

		/// @brief Sets this listener as enabled. Enabled listeners receive ticks and call their callbacks
		/// @param isEnabled If true, this listener will receive ticks
		constexpr void SetIsEnabled(bool isEnabled) noexcept { _isEnabled = isEnabled; }

		/// @brief Gets if this listener is enabled
		/// @return True if this listener is responding to ticks
		constexpr bool GetIsEnabled() const noexcept { return _isEnabled; }

	private:
		friend class MainLoop;

		/// @brief Called by the main loop every tick
		/// @param deltaTime The time between ticks (in seconds)
		void Tick(double deltaTime);
	};
}