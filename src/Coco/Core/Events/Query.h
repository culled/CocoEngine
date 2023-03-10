#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "QueryHandler.h"

namespace Coco
{
	/// <summary>
	/// An query that can fire to listening handlers
	/// </summary>
	/// <typeparam name="T">The return type of the query</typeparam>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ReturnType, typename ... Args>
	class COCOAPI Query
	{
	public:
		using HandlerFunctionType = QueryHandler<ReturnType, Args...>::HandlerFunctionType;
		using HandlerType = QueryHandler<ReturnType, Args...>;
		using HandlerID = uint;

	private:
		List<Ref<HandlerType>> _handlers;

	public:
		Query() = default;

		virtual ~Query()
		{
			_handlers.Clear();
		}

		Query(const Query&) = delete;
		Query(Query&&) = delete;

		Query& operator=(const Query&) = delete;
		Query& operator=(Query&&) = delete;

		/// <summary>
		/// Adds an instance and member function handler
		/// </summary>
		/// <typeparam name="ObjectType">The type of instance</typeparam>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>The ID of the handler</returns>
		template<typename ObjectType>
		Ref<HandlerType> AddHandler(ObjectType* object, ReturnType(ObjectType::* function)(Args...))
		{
			return AddHandlerImpl(new ObjectQueryHandler<ObjectType, ReturnType, Args...>(object, function));
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
		/// <typeparam name="ObjectType">The type of instance</typeparam>
		/// <param name="object">The instanced object</param>
		/// <param name="function">The handler function pointer</param>
		/// <returns>True if the handler was found and removed</returns>
		template<typename ObjectType>
		bool RemoveHandler(ObjectType* object, ReturnType(ObjectType::* function)(Args...))
		{
			ObjectQueryHandler<ObjectType, ReturnType, Args...> handler(object, function);

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
		/// Invokes this query and sets the value to the first handler's return value
		/// </summary>
		/// <param name="...params">The parameters for the query</param>
		/// <returns>If the query was handled</returns>
		bool Invoke(ReturnType* value, Args... params)
		{
			List<Ref<HandlerType>> handlersCopy = _handlers;

			for (const Ref<HandlerType>& handler : handlersCopy)
			{
				if ((*handler)(value, params...))
				{
					return true;
				}
			}

			return false;
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

		bool operator()(ReturnType& value, Args... params)
		{
			return Invoke(value, params...);
		}

	private:
		/// <summary>
		/// Adds a handler pointer to the handlers for this query
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