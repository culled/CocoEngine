#pragma once

#include <Coco/Core/Corepch.h>
#include <Coco/Core/Defines.h>

namespace Coco
{
	template <typename ValueType, typename ... ArgTypes>
	class QueryHandler;

	/// @brief A query that can invoke QueryHandlers for a value
	/// @tparam ValueType The type of queried value
	/// @tparam ...ArgTypes The types of arguments for the query
	template<typename ValueType, typename ... ArgTypes>
	class Query
	{
	public:
		using HandlerType = QueryHandler<ValueType, ArgTypes...>;

		friend class HandlerType;

	private:
		std::vector<HandlerType*> _handlers;

	public:
		Query() = default;
		~Query()
		{
			std::vector<HandlerType*> tempHandlers(_handlers);

			// Disconnect all handlers
			for (auto it = tempHandlers.begin(); it != tempHandlers.end(); it++)
			{
				if (*it)
					(*it)->Disconnect();
			}
		}

		/// @brief Invokes this query with the given arguments
		/// @param value The queried value
		/// @param ...args The arguments
		/// @return True if this query was handled
		bool Invoke(ValueType& value, ArgTypes ... args)
		{
			bool handled = false;
			std::vector<HandlerType*> handlersTemp(_handlers);

			for (auto it = handlersTemp.begin(); it != handlersTemp.end(); it++)
			{
				HandlerType* handler = *it;

				if (handler)
				{
					handled = handler->Invoke(value, args...);

					if (handled)
						break;
				}
			}

			return handled;
		}

		/// @brief Gets the number of handlers registered to this query
		/// @return The number of query handlers
		size_t GetHandlerCount() const { return _handlers.size(); }

	private:
		/// @brief Adds a handler to this query
		/// @param handler The handler
		void AddHandler(HandlerType& handler)
		{
			_handlers.insert(_handlers.begin(), handler);
		}

		/// @brief Removes a handler from this query
		/// @param handler The handler
		void RemoveHandler(HandlerType& handler)
		{
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);

			if (it != _handlers.end())
				_handlers.erase(it);
		}
	};

	/// @brief A handler for queries
	/// @tparam ...ValueType The type of queried value
	/// @tparam ...ArgTypes The types of arguments this handler supports
	template<typename ValueType, typename ... ArgTypes>
	class QueryHandler
	{
	public:
		using QueryType = Query<ValueType, ArgTypes...>;
		friend class QueryType;

	public:
		using CallbackFunction = std::function<bool(ValueType&, ArgTypes...)>;

	private:
		QueryType* _query;
		CallbackFunction _callback;

	public:
		QueryHandler() :
			_query(nullptr),
			_callback(nullptr)
		{}

		QueryHandler(CallbackFunction callback) :
			_query(nullptr)
		{
			SetCallback(callback);
		}

		template<typename ObjectType>
		QueryHandler(ObjectType* instance, bool(ObjectType::* callback)(ValueType&, ArgTypes...)) :
			_query(nullptr)
		{
			SetCallback(instance, callback);
		}

		~QueryHandler()
		{
			Disconnect();
		}

		/// @brief Connects this handler to a query.
		/// NOTE: This will disconnect from the current query if one is already connected
		/// @param source The query
		void Connect(QueryType& source)
		{
			if (_query)
				Disconnect();

			_query = &source;
			_query->AddHandler(*this);
		}

		/// @brief Disconnects from the connected query
		void Disconnect()
		{
			if (_query)
				_query->RemoveHandler(*this);

			_query = nullptr;
		}

		/// @brief Sets the callback function that this handler will invoke when the connected query is fired
		/// @param callback The callback function
		void SetCallback(CallbackFunction callback)
		{
			_callback = callback;
		}

		/// @brief Sets the callback function that this handler will invoke when the connected query is fired
		/// @tparam ObjectType The type of object
		/// @param instance The object instance
		/// @param callback The callback member function
		template<typename ObjectType>
		void SetCallback(ObjectType* instance, bool(ObjectType::* callback)(ValueType&, ArgTypes...))
		{
			// HACK: Binding doesn't seem to work in this situation, so just use a lambda
			_callback = [instance, callback](ValueType& value, ArgTypes&&... args) { return (instance->*callback)(value, std::forward<ArgTypes>(args)...); };

			//_callback = std::bind(callback, instance, std::placeholders::_1);
		}

		/// @brief Gets if this handler is connected to a query
		/// @return True if this handler is connected to a query
		bool IsConnected() const { return _query != nullptr; }

	private:
		/// @brief Invokes the callback function
		/// @param value The queried value
		/// @param ...args The query arguments
		/// @return True if the query was handled
		bool Invoke(ValueType& value, ArgTypes ... args)
		{
			if (!_callback)
				return false;

			return _callback(value, args...);
		}
	};
}