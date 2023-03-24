#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "QueryHandler.h"

namespace Coco
{
	/// @brief An event that can fire to listening handlers
	/// @tparam ...Args 
	template<typename ... Args>
	class COCOAPI Event
	{
	public:
		/// @brief The type for an event handler function
		using HandlerFunctionType = QueryHandler<bool, Args...>::HandlerFunctionType;

		/// @brief The type for event handlers
		using HandlerType = QueryHandler<bool, Args...>;

		/// @brief The type for a event handler ID
		using HandlerID = uint64_t;

	private:
		List<Ref<HandlerType>> _handlers;

	public:

		Event() = default;

		virtual ~Event()
		{
			_handlers.Clear();
		}

		Event(const Event&) = delete;
		Event(Event&&) = delete;

		Event& operator=(const Event&) = delete;
		Event& operator=(Event&&) = delete;

		/// @brief Adds an instance and member function event handler
		/// @tparam ObjectType 
		/// @param object The instanced object
		/// @param function The event handler function pointer
		/// @return A handler for the event
		template<typename ObjectType>
		Ref<HandlerType> AddHandler(ObjectType* object, bool(ObjectType::* function)(Args...))
		{
			Ref<ObjectQueryHandler<ObjectType, bool, Args...>> handler = CreateRef<ObjectQueryHandler<ObjectType, bool, Args...>>(object, function);
			AddHandler(handler);
			return handler;
		}

		/// @brief Adds a generic event function handler
		/// @param handlerFunction The event handler function
		/// @return A handler for the event
		Ref<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			Ref<HandlerType> handler = CreateRef<HandlerType>(handlerFunction);
			AddHandler(handler);
			return handler;
		}

		/// @brief Adds an existing event handler
		/// @param handler A handler reference
		void AddHandler(const Ref<HandlerType>& handler)
		{
			_handlers.Insert(0, handler);
		}

		/// @brief Removes an instance and member function event handler
		/// @tparam ObjectType 
		/// @param object The instanced object
		/// @param function The handler function pointer
		/// @return True if the handler was found and removed
		template<typename ObjectType>
		bool RemoveHandler(ObjectType* object, bool(ObjectType::* function)(Args...)) noexcept
		{
			auto it = _handlers.Find([object, function](const Ref<HandlerType>& other) noexcept {
				if (ObjectQueryHandler<ObjectType, bool, Args...>* otherPtr = dynamic_cast<ObjectQueryHandler<ObjectType, bool, Args...>*>(other.get()))
				{
					return otherPtr->Equals(object, function);
				}

				return false;
				});

			if (it != _handlers.end())
			{
				return _handlers.Remove(it);
			}

			return false;
		}

		/// @brief Removes an event handler by its ID
		/// @param handlerID The ID of the handler
		/// @return True if the handler was found and removed
		bool RemoveHandler(HandlerID handlerID) noexcept
		{
			auto it = _handlers.Find([handlerID](const Ref<HandlerType>& other) noexcept {
				return other->ID == handlerID;
				});

			if (it != _handlers.end())
				return _handlers.Remove(it);

			return false;
		}

		/// @brief Removes an event handler
		/// @param handler The handler to remove
		/// @return True if the handler was found and removed
		bool RemoveHandler(const Ref<HandlerType>& handler) noexcept
		{
			return _handlers.Remove(handler);
		}

		/// @brief Invokes this event
		/// @param ...params The parameters for the event
		/// @return True if this event was handled
		bool Invoke(Args... params)
		{
			List<Ref<HandlerType>> handlersCopy = _handlers;

			bool handled = false;

			for (const Ref<HandlerType>& handler : handlersCopy)
			{
				(*handler)(&handled, params...);

				if (handled)
					break;
			}

			return handled;
		}

		void operator()(Args... params)
		{
			Invoke(params...);
		}

		Ref<HandlerType> operator +=(const HandlerFunctionType& handlerFunction)
		{
			return AddHandler(handlerFunction);
		}

		bool operator -=(const HandlerID& handlerID)
		{
			return RemoveHandler(handlerID);
		}

		bool operator -=(const Ref<HandlerType>& handler)
		{
			return RemoveHandler(handler);
		}
	};
}

