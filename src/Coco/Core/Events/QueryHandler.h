#pragma once

#include <Coco/Core/Core.h>

#include <functional>
#include <atomic>

namespace Coco
{
	/// <summary>
	/// A handler for queries
	/// </summary>
	/// <typeparam name="ReturnType">The event return type</typeparam>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ReturnType, typename ... Args>
	class COCOAPI QueryHandler
	{
	public:
		using HandlerFunctionType = std::function<ReturnType(Args...)>;

	private:
		static std::atomic<uint32_t> _handlerIdCounter;

		HandlerFunctionType _handler;
		uint32_t _id;
		bool _isBlocked = false;

	public:
		QueryHandler(const HandlerFunctionType& handlerFunction) : _handler(handlerFunction)
		{
			_id = ++_handlerIdCounter;
		}

		virtual ~QueryHandler() = default;

		bool operator()(ReturnType* value, Args ... params) const
		{
			if (_handler && !_isBlocked)
			{
				if (value != nullptr)
				{
					*value = _handler(params...);
				}
				else
				{
					_handler(params...);
				}

				return true;
			}

			return false;
		}

		virtual bool operator==(QueryHandler<ReturnType, Args...>* other) const noexcept { return _id == other->_id; }

		/// <summary>
		/// Gets this handler's ID
		/// </summary>
		/// <returns>This handler's ID</returns>
		unsigned int GetID() const noexcept { return _id; }

		/// <summary>
		/// Sets the blocked state of this handler.
		/// Blocked handlers do not receive events
		/// </summary>
		/// <param name="isBlocked">The blocked state</param>
		void SetIsBlocked(bool isBlocked) noexcept { _isBlocked = true; }
	};

	template <typename ReturnType, typename... Args>
	std::atomic_uint QueryHandler<ReturnType, Args...>::_handlerIdCounter(0);

	/// <summary>
	/// Handler for events that use an instance and member function
	/// </summary>
	/// <typeparam name="ObjectType">The type of instance</typeparam>
	/// <typeparam name="ReturnType">The return type of the event</typeparam>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ObjectType, typename ReturnType, typename ... Args>
	class ObjectQueryHandler : public QueryHandler<ReturnType, Args...>
	{
		using ObjectHandlerFunctionType = ReturnType(ObjectType::*)(Args...);

	private:
		ObjectType* _instance;
		ObjectHandlerFunctionType _function;

	public:
		ObjectQueryHandler(ObjectType* instance, ObjectHandlerFunctionType function) :
			QueryHandler<ReturnType, Args...>(std::bind(function, instance)),
			_instance(instance),
			_function(function)
		{}

		bool operator==(QueryHandler<ReturnType, Args...>* other) const noexcept override
		{
			if (ObjectQueryHandler<ObjectType, ReturnType, Args...>* otherPtr = dynamic_cast<ObjectQueryHandler<ObjectType, ReturnType, Args...>*>(other))
			{
				return _instance == otherPtr->_instance && _function == otherPtr->_function;
			}

			return false;
		}

		bool Equals(ObjectType* instance, ObjectHandlerFunctionType function) const noexcept
		{
				return instance == _instance && function == _function;
		}
	};
}