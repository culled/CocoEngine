#pragma once

#include <Coco/Core/Corepch.h>
#include <Coco/Core/Defines.h>

namespace Coco
{
	template <typename ... ArgTypes>
	class EventHandler;

	/// @brief An event that can invoke EventHandlers
	/// @tparam ...ArgTypes The types of arguments for the event
	template<typename ... ArgTypes>
	class Event
	{
	public:
		using HandlerType = EventHandler<ArgTypes...>;

		friend class HandlerType;

	private:
		std::vector<HandlerType*> _handlers;

	public:
		Event() = default;
		~Event()
		{
			std::vector<HandlerType*> tempHandlers(_handlers);

			// Disconnect all handlers
			for (auto it = tempHandlers.begin(); it != tempHandlers.end(); it++)
			{
				if(*it)
					(*it)->Disconnect();
			}
		}

		/// @brief Invokes this event with the given arguments
		/// @param ...args The arguments
		/// @return True if this event was handled
		bool Invoke(ArgTypes ... args)
		{
			bool handled = false;
			std::vector<HandlerType*> handlersTemp(_handlers);

			for (auto it = handlersTemp.begin(); it != handlersTemp.end(); it++)
			{
				HandlerType* handler = *it;

				if (handler)
				{
					handled = handler->Invoke(args...);

					if (handled)
						break;
				}
			}

			return handled;
		}

		/// @brief Gets the number of handlers registered to this event
		/// @return The number of event handlers
		size_t GetHandlerCount() const { return _handlers.size(); }

	private:
		/// @brief Adds a handler to this event
		/// @param handler The handler
		void AddHandler(HandlerType* handler)
		{
			_handlers.insert(_handlers.begin(), handler);
		}

		/// @brief Removes a handler from this event
		/// @param handler The handler
		void RemoveHandler(HandlerType* handler)
		{
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);

			if (it != _handlers.end())
				_handlers.erase(it);
		}
	};

	/// @brief A handler for events
	/// @tparam ...ArgTypes The types of arguments this handler supports
	template<typename ... ArgTypes>
	class EventHandler
	{
	public:
		using EventType = Event<ArgTypes...>;

		friend class EventType;

	public:
		using CallbackFunction = std::function<bool(ArgTypes...)>;

	private:
		EventType* _event;
		CallbackFunction _callback;

	public:
		EventHandler() :
			_event(nullptr),
			_callback(nullptr)
		{}

		EventHandler(CallbackFunction callback) :
			_event(nullptr)
		{
			SetCallback(callback);
		}

		template<typename ObjectType>
		EventHandler(ObjectType* instance, bool(ObjectType::* callback)(ArgTypes...)) :
			_event(nullptr)
		{
			SetCallback(instance, callback);
		}

		~EventHandler()
		{
			Disconnect();
		}

		/// @brief Connects this handler to an event.
		/// NOTE: This will disconnect from the current event if one is already connected
		/// @param source The event
		void Connect(EventType* source)
		{
			if (!source)
				return;

			if (_event)
				Disconnect();

			_event = source;
			_event->AddHandler(this);
		}

		/// @brief Disconnects from the connected event
		void Disconnect()
		{
			if (_event)
				_event->RemoveHandler(this);

			_event = nullptr;
		}

		/// @brief Sets the callback function that this handler will invoke when the connected event is fired
		/// @param callback The callback function
		void SetCallback(CallbackFunction callback)
		{
			_callback = callback;
		}

		/// @brief Sets the callback function that this handler will invoke when the connected event is fired
		/// @tparam ObjectType The type of object
		/// @param instance The object instance
		/// @param callback The callback member function
		template<typename ObjectType>
		void SetCallback(ObjectType* instance, bool(ObjectType::* callback)(ArgTypes...))
		{
			_callback = std::bind(callback, instance, std::placeholders::_1);
		}

		/// @brief Gets if this handler is connected to an event
		/// @return True if this handler is connected to an event
		bool IsConnected() const { return _event != nullptr; }

	private:
		/// @brief Invokes the callback function
		/// @param ...args The event arguments
		/// @return True if the event was handled
		bool Invoke(ArgTypes ... args)
		{
			if (!_callback)
				return false;

			return _callback(args...);
		}
	};
}