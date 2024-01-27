#pragma once

#include "../Corepch.h"
#include "../Defines.h"

#include "TickInfo.h"

namespace Coco
{
	/// @brief Can be registered to a MainLoop to listen for ticks and invoke a callback function when ticked
	class TickListener
	{
		friend class MainLoop;

	public:
		/// @brief The signature of a tick callback function
		using CallbackFunction = std::function<void(const TickInfo&)>;

		/// @brief The priority of this listener. Lower priorities tick earlier than higher priorities
		const int Priority;

	public:
		TickListener(CallbackFunction callback, int priority);

		template<typename ObjectType>
		TickListener(ObjectType* instance, void(ObjectType::* callback)(const TickInfo&), int priority) :
			TickListener(std::bind(callback, instance, std::placeholders::_1), priority)
		{}

		/// @brief Sets the enabled state of this listener
		/// @param enabled If true, this listener will dispatch ticks to the callback
		void SetEnabled(bool enabled);

		/// @brief Gets the enabled state of this listener
		/// @return The enabled state
		bool IsEnabled() const { return _enabled; }

		/// @brief Sets the minimum period between ticks that this listener will dispatch
		/// @param period The minimum period between ticks
		/// @param useTimescale If true, the main loop's timescale will be taken into account for the tick period
		void SetTickPeriod(double period, bool useTimescale);

		/// @brief Gets the tick period of this listener
		/// @return The tick period
		double GetTickPeriod() const { return _tickPeriod; }

		/// @brief Sets the callback function that this listener will invoke when a tick should be dispatched
		/// @param callback The callback function
		void SetCallback(CallbackFunction callback);

		/// @brief Sets the callback function that this listener will call when a tick should be dispatched
		/// @tparam ObjectType The type of object
		/// @param instance The instance of the object
		/// @param callback The callback member function
		template<typename ObjectType>
		void SetCallback(ObjectType* instance, void(ObjectType::* callback)(const TickInfo&))
		{
			SetCallback(std::bind(callback, instance, std::placeholders::_1));
		}

	private:
		bool _enabled;
		double _tickPeriod;
		bool _useTimescale;
		double _timeSinceLastTick;
		CallbackFunction _callback;

	private:
		/// @brief Invokes the callback function if it should be dispatched
		/// @param tickInfo The info about the current tick
		void Invoke(const TickInfo& tickInfo);
	};
}