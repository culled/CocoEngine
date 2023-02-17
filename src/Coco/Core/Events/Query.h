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
		using HandlerID = unsigned int;

	private:
		List<Ref<HandlerType>> Handlers;

	public:
		virtual ~Query()
		{
			Handlers.Clear();
		}

		/// <summary>
		/// Adds an instance and member function handler to this query
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
		/// Adds a generic function handler to this query
		/// </summary>
		/// <param name="handlerFunction">The handler function</param>
		/// <returns>The ID of the handler</returns>
		Ref<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			return AddHandlerImpl(new HandlerType(handlerFunction));
		}

		/// <summary>
		/// Adds a handler to this query
		/// </summary>
		/// <param name="handler">A handler reference</param>
		void AddHandler(const Ref<HandlerType>& handler)
		{
			Handlers.Add(handler);
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

			auto it = std::find_if(Handlers.begin(), Handlers.end(), [handler](const Ref<HandlerType>& other) {
				return handler == other.get();
				});

			if (it != Handlers.end())
			{
				Handlers.Erase(it);
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
			auto it = std::find_if(Handlers.begin(), Handlers.end(), [handlerId](const Ref<HandlerType>& other) {
				return other->GetID() == handlerId;
				});

			if (it != Handlers.end())
			{
				Handlers.Erase(it);
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
			return Handlers.Remove(handler);
		}

		/// <summary>
		/// Invokes this query and sets the value to the first handler's return value
		/// </summary>
		/// <param name="...params">The parameters for the query</param>
		/// <returns>If the query was handled</returns>
		virtual bool Invoke(ReturnType* value, Args... params)
		{
			List<Ref<HandlerType>> handlersCopy = GetHandlerListCopy();

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
			return Invoke(value, std::forward<Args>(params)...);
		}

	protected:
		/// <summary>
		/// Gets a copy of the handler list
		/// </summary>
		/// <returns>A copy of the handler list</returns>
		List<Ref<HandlerType>> GetHandlerListCopy()
		{
			return Handlers;
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
			Handlers.Add(handlerRef);
			return handlerRef;
		}
	};
}