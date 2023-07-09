#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "QueryHandler.h"

namespace Coco
{
	/// @brief An event that can fire to listening handlers
	/// @tparam ...Args The types of event arguments
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
		List<SharedRef<HandlerType>> _handlers;

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

		/// @brief Adds an event handler for a member function of an object
		/// @tparam ObjectType The type of object
		/// @param object The object
		/// @param function The event handler function
		/// @return A handler for the event
		template<typename ObjectType>
		WeakSharedRef<HandlerType> AddHandler(ObjectType* object, bool(ObjectType::* function)(Args...))
		{
			return AddHandler(CreateSharedRef<ObjectQueryHandler<ObjectType, bool, Args...>>(object, function));
		}

		/// @brief Adds a generic event handler function
		/// @param handlerFunction The event handler function
		/// @return A handler for the event
		WeakSharedRef<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			return AddHandler(CreateSharedRef<HandlerType>(handlerFunction));
		}

		/// @brief Removes an event handler for a member function of an object
		/// @tparam ObjectType The type of object
		/// @param object The object
		/// @param function The event handler function
		/// @return True if the handler was found and removed
		template<typename ObjectType>
		bool RemoveHandler(ObjectType* object, bool(ObjectType::* function)(Args...)) noexcept
		{
			auto it = _handlers.Find([object, function](const SharedRef<HandlerType>& other) noexcept {
				if (const ObjectQueryHandler<ObjectType, bool, Args...>* otherPtr = dynamic_cast<const ObjectQueryHandler<ObjectType, bool, Args...>*>(other.Get()))
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
			auto it = _handlers.Find([handlerID](const SharedRef<HandlerType>& other) noexcept {
				return other->ID == handlerID;
				});

			if (it != _handlers.end())
				return _handlers.Remove(it);

			return false;
		}

		/// @brief Removes an event handler
		/// @param handler The handler to remove
		/// @return True if the handler was found and removed
		bool RemoveHandler(const WeakSharedRef<HandlerType>& handler) noexcept
		{
			if (!handler.IsValid())
				return false;

			SharedRef<HandlerType> lock = handler.Lock();
			return RemoveHandler(lock->ID);
		}

		/// @brief Invokes this event
		/// @param ...params The parameters for the event
		/// @return True if this event was handled
		bool Invoke(Args... params)
		{
			List<SharedRef<HandlerType>> handlersCopy = _handlers;

			bool handled = false;

			for (SharedRef<HandlerType>& handler : handlersCopy)
			{
				(*handler.Get())(&handled, params...);

				if (handled)
					break;
			}

			return handled;
		}

		void operator()(Args... params)
		{
			Invoke(params...);
		}

		WeakSharedRef<HandlerType> operator +=(const HandlerFunctionType& handlerFunction)
		{
			return AddHandler(handlerFunction);
		}

		bool operator -=(const HandlerID& handlerID)
		{
			return RemoveHandler(handlerID);
		}

		bool operator -=(const WeakSharedRef<HandlerType>& handler)
		{
			return RemoveHandler(handler);
		}

		private:
			/// @brief Adds an existing event handler
			/// @param handler A handler reference
			WeakSharedRef<HandlerType> AddHandler(SharedRef<HandlerType>&& handler)
			{
				_handlers.Insert(0, std::forward<SharedRef<HandlerType>>(handler));
				return _handlers.First();
			}
	};
}

