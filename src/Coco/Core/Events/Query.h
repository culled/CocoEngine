#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "QueryHandler.h"

namespace Coco
{
	/// @brief A query that can fire to listening handlers
	/// @tparam ...Args Types of query arguments
	/// @tparam ReturnType The type that the query returns
	template<typename ReturnType, typename ... Args>
	class COCOAPI Query
	{
	public:
		/// @brief The type for a query handler function
		using HandlerFunctionType = QueryHandler<ReturnType, Args...>::HandlerFunctionType;

		/// @brief The type for query handlers
		using HandlerType = QueryHandler<ReturnType, Args...>;
		
		/// @brief The type for a query handler ID
		using HandlerID = uint64_t;

	private:
		List<SharedRef<HandlerType>> _handlers;

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

		/// @brief Adds a query handler for a member function of an object
		/// @tparam ObjectType The type of object
		/// @param object The object
		/// @param function The query handler function
		/// @return A handler for the query
		template<typename ObjectType>
		WeakSharedRef<HandlerType> AddHandler(ObjectType* object, ReturnType(ObjectType::* function)(Args...))
		{
			AddHandler(CreateSharedRef<ObjectQueryHandler<ObjectType, ReturnType, Args...>>(object, function));
			return _handlers.First();
		}

		/// @brief Adds a generic query handler function
		/// @param handlerFunction The query handler function
		/// @return A handler for the query
		Ref<HandlerType> AddHandler(const HandlerFunctionType& handlerFunction)
		{
			AddHandler(CreateSharedRef<HandlerType>(handlerFunction));
			return _handlers.First();
		}

		/// @brief Removes a query handler for a member function of an object
		/// @tparam ObjectType The type of object
		/// @param object The object
		/// @param function The query handler function
		/// @return True if the handler was found and removed
		template<typename ObjectType>
		bool RemoveHandler(ObjectType* object, ReturnType(ObjectType::* function)(Args...)) noexcept
		{
			auto it = _handlers.Find([object, function](const SharedRef<HandlerType>& other) noexcept {
				if (ObjectQueryHandler<ObjectType, ReturnType, Args...>* otherPtr = dynamic_cast<ObjectQueryHandler<ObjectType, ReturnType, Args...>*>(other.Get()))
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

		/// @brief Removes a query handler by its ID
		/// @param handlerID The ID of the handler
		/// @return True if the query handler was found and removed
		bool RemoveHandler(HandlerID handlerID) noexcept
		{
			auto it = _handlers.Find([handlerID](const SharedRef<HandlerType>& other) noexcept {
				return other->ID == handlerID;
				});

			if (it != _handlers.end())
				return _handlers.Remove(it);

			return false;
		}

		/// @brief Removes a query handler
		/// @param handler The query handler to remove
		/// @return True if the handler was found and removed
		bool RemoveHandler(const WeakSharedRef<HandlerType>& handler) noexcept
		{
			if (!handler.IsValid())
				return false;

			SharedRef<HandlerType> lock = handler.Lock();
			return RemoveHandler(lock->ID);
		}

		/// @brief Invokes this query
		/// @param value The value that will be modified by the query
		/// @param ...params The parameters for the query
		/// @return If the query was handled
		bool Invoke(ReturnType* value, Args... params)
		{
			List<SharedRef<HandlerType>> handlersCopy = _handlers;

			for (const SharedRef<HandlerType>& handler : handlersCopy)
			{
				if ((*handler)(value, params...))
					return true;
			}

			return false;
		}

		bool operator()(ReturnType* value, Args... params)
		{
			return Invoke(value, params...);
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
		/// @brief Adds an existing query handler
		/// @param handler A query handler reference
		void AddHandler(SharedRef<HandlerType>&& handler)
		{
			_handlers.Insert(0, std::forward<SharedRef<HandlerType>>(handler));
		}
	};
}