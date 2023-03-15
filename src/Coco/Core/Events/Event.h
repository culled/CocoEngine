#pragma once

#include <Coco/Core/Core.h>
#include "QueryHandler.h"

namespace Coco
{
	/// <summary>
	/// An event that can fire to listening handlers
	/// </summary>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ... Args>
	class COCOAPI Event
	{
	public:
		using HandlerFunctionType = QueryHandler<bool, Args...>::HandlerFunctionType;
		using HandlerType = QueryHandler<bool, Args...>;
		using HandlerID = uint;

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

		/// <summary>
		/// Adds an instance and member function handler
		/// </summary>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>The ID of the handler</returns>
		template<typename ObjectType>
		Ref<HandlerType> AddHandler(ObjectType* object, bool(ObjectType::* function)(Args...))
		{
			return AddHandlerImpl(new ObjectQueryHandler<ObjectType, bool, Args...>(object, function));
		}

		/// <summary>
		/// Adds a generic function handler
		/// </summary>
		/// <param name="handlerFunction">The handler function</param>
		/// <returns>The ID of the handler</returns>
		Ref<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			return AddHandlerImpl(new HandlerType(handlerFunction));
		}

		/// <summary>
		/// Adds an existing handler
		/// </summary>
		/// <param name="handler">A handler reference</param>
		void AddHandler(const Ref<HandlerType>& handler)
		{
			_handlers.Insert(0, handler);
		}

		/// <summary>
		/// Removes an instance and member function handler
		/// </summary>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>True if the handler was found and removed</returns>
		template<typename ObjectType>
		bool RemoveHandler(ObjectType* object, bool(ObjectType::* function)(Args...))
		{
			ObjectQueryHandler<ObjectType, bool, Args...> handler(object, function);

			auto it = std::find_if(_handlers.begin(), _handlers.end(), [handler](const Ref<HandlerType>& other) {
				return handler == other.get();
				});

			if (it != _handlers.end())
			{
				_handlers.Erase(it);
				return true;
			}

			return false;
		}

		/// <summary>
		/// Removes a handler by its ID
		/// </summary>
		/// <param name="handlerId">The ID of the handler, received as the return value of AddHandler()</param>
		/// <returns>True if the handler was found and removed</returns>
		bool RemoveHandler(HandlerID handlerId)
		{
			auto it = std::find_if(_handlers.begin(), _handlers.end(), [handlerId](const Ref<HandlerType>& other) {
				return other->GetID() == handlerId;
				});

			if (it != _handlers.end())
			{
				_handlers.Erase(it);
				return true;
			}

			return false;
		}

		/// <summary>
		/// Removes a handler
		/// </summary>
		/// <param name="handler">The handler to remove</param>
		/// <returns>True if the handler was found and removed</returns>
		bool RemoveHandler(const Ref<HandlerType>& handler)
		{
			return _handlers.Remove(handler);
		}

		/// <summary>
		/// Invokes this event
		/// </summary>
		/// <param name="...params">The parameters for the event</param>
		/// <returns>True if this event was handled</returns>
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

	private:
		/// <summary>
		/// Adds a handler pointer to the handlers for this event
		/// </summary>
		/// <param name="handler">The handler to add</param>
		/// <returns>A reference to the handler</returns>
		Ref<HandlerType> AddHandlerImpl(HandlerType* handler)
		{
			Ref<HandlerType> handlerRef(handler);
			_handlers.Insert(0, handlerRef);
			return handlerRef;
		}
	};
}

