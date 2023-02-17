#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "EventArgs.h"
#include "EventHandler.h"

#include <functional>

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
		using HandlerFunctionType = std::function<bool(Args...)>;
		using HandlerType = EventHandler<Args...>;
		using HandlerID = unsigned int;

	private:
		List<Ref<HandlerType>> _handlers;

	public:
		~Event()
		{
			_handlers.Clear();
		}

		/// <summary>
		/// Adds an instance and member function handler
		/// </summary>
		/// <typeparam name="T">The type of instance</typeparam>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>The ID of the handler</returns>
		template<typename T>
		Ref<HandlerType> AddHandler(T* object, bool(T::* function)(Args...))
		{
			return AddHandlerImpl(new ObjectEventHandler<T, Args...>(object, function));
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
		/// Adds a handler to this event
		/// </summary>
		/// <param name="handler">A handler reference</param>
		void AddHandler(const Ref<HandlerType>& handler)
		{
			_handlers.Add(handler);
		}

		/// <summary>
		/// Removes an instance and member function handler
		/// </summary>
		/// <typeparam name="T">The type of instance</typeparam>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>True if the handler was found and removed</returns>
		template<typename T>
		bool RemoveHandler(T* object, bool(T::* function)(Args...))
		{
			ObjectEventHandler<T, Args...> handler(object, function);

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
		void Invoke(Args... params)
		{
			List<Ref<HandlerType>> handlersCopy = _handlers;

			for (const Ref<HandlerType>& handler : handlersCopy)
			{
				// Stop propagating if the event has been handled
				if((*handler)(params...))
					break;
			}
		}

		Ref<Event::HandlerType> operator +=(const Event::HandlerFunctionType& handlerFunction)
		{
			return AddHandler(handlerFunction);
		}

		bool operator -=(const Event::HandlerID& handlerID)
		{
			return RemoveHandler(handlerID);
		}

		bool operator -=(const Ref<HandlerType>& handler)
		{
			return RemoveHandler(handler);
		}

		void operator()(Args... params) { Invoke(std::forward<Args>(params)...); }

	private:
		/// <summary>
		/// Adds a handler pointer to the handlers for this event
		/// </summary>
		/// <param name="handler">The handler to add</param>
		/// <returns>A reference to the handler</returns>
		Ref<HandlerType> AddHandlerImpl(HandlerType* handler)
		{
			Ref<HandlerType> handlerRef(handler);
			_handlers.Add(handlerRef);
			return handlerRef;
		}
	};
}

