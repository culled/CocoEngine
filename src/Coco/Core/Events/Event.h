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
			Ref<ObjectQueryHandler<ObjectType, bool, Args...>> handler = CreateRef<ObjectQueryHandler<ObjectType, bool, Args...>>(object, function);
			AddHandler(handler);
			return handler;
		}

		/// <summary>
		/// Adds a generic function handler
		/// </summary>
		/// <param name="handlerFunction">The handler function</param>
		/// <returns>The ID of the handler</returns>
		Ref<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			Ref<HandlerType> handler = CreateRef<HandlerType>(handlerFunction);
			AddHandler(handler);
			return handler;
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

		/// <summary>
		/// Removes a handler by its ID
		/// </summary>
		/// <param name="handlerId">The ID of the handler, received as the return value of AddHandler()</param>
		/// <returns>True if the handler was found and removed</returns>
		bool RemoveHandler(HandlerID handlerId) noexcept
		{
			auto it = _handlers.Find([handlerId](const Ref<HandlerType>& other) noexcept {
				return other->GetID() == handlerId;
				});

			if (it != _handlers.end())
			{
				return _handlers.Remove(it);
			}

			return false;
		}

		/// <summary>
		/// Removes a handler
		/// </summary>
		/// <param name="handler">The handler to remove</param>
		/// <returns>True if the handler was found and removed</returns>
		bool RemoveHandler(const Ref<HandlerType>& handler) noexcept
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
	};
}

