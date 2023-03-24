#pragma once

#include <Coco/Core/Core.h>

#include <functional>
#include <atomic>

namespace Coco
{	
	/// @brief A handler for queries/events
	/// @tparam ReturnType 
	/// @tparam ...Args 
	template<typename ReturnType, typename ... Args>
	class COCOAPI QueryHandler
	{
	public:
		/// @brief The function signature for this handler
		using HandlerFunctionType = std::function<ReturnType(Args...)>;

		/// @brief The type for a query/event handler ID
		using HandlerID = uint64_t;

		/// @brief The ID of this handler
		const HandlerID ID;

	private:
		static std::atomic<HandlerID> _handlerIdCounter;

		HandlerFunctionType _handler;
		bool _isBlocked = false;

	public:
		QueryHandler(const HandlerFunctionType& handlerFunction) : _handler(handlerFunction), ID(++_handlerIdCounter)
		{}

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

		virtual bool operator==(QueryHandler<ReturnType, Args...>* other) const noexcept { return ID == other->ID; }

		/// @brief Sets the blocked state of this handler. Blocked handlers do not receive queries/events
		/// @param isBlocked The blocked state
		constexpr void SetIsBlocked(bool isBlocked) noexcept { _isBlocked = true; }

		/// @brief Gets the blocked state of this handler
		/// @return True if this handler is blocked
		constexpr bool GetIsBlocked() const noexcept { return _isBlocked; }
	};

	template <typename ReturnType, typename... Args>
	std::atomic_uint64_t QueryHandler<ReturnType, Args...>::_handlerIdCounter(0);

	/// @brief Handler for queries/events that use an instance and member function
	/// @tparam ObjectType 
	/// @tparam ReturnType 
	/// @tparam ...Args 
	template<typename ObjectType, typename ReturnType, typename ... Args>
	class ObjectQueryHandler : public QueryHandler<ReturnType, Args...>
	{
		/// @brief The function signature for this handler
		using ObjectHandlerFunctionType = ReturnType(ObjectType::*)(Args...);

	private:
		ObjectType* _instance;
		ObjectHandlerFunctionType _function;

	public:
		ObjectQueryHandler(ObjectType* instance, ObjectHandlerFunctionType function) :
			QueryHandler<ReturnType, Args...>(std::bind(function, instance)), _instance(instance), _function(function)
		{}

		/// @brief Returns if this handler references the same instance and member function as the given pair
		/// @param instance The object instance
		/// @param function The function pointer
		/// @return True if this handler references the object and function
		bool Equals(ObjectType* instance, ObjectHandlerFunctionType function) const noexcept
		{
			return instance == _instance && function == _function;
		}

		bool operator==(QueryHandler<ReturnType, Args...>* other) const noexcept override
		{
			if (ObjectQueryHandler<ObjectType, ReturnType, Args...>* otherPtr = dynamic_cast<ObjectQueryHandler<ObjectType, ReturnType, Args...>*>(other))
			{
				return Equals(otherPtr->_instance, otherPtr->_function);
			}

			return false;
		}
	};
}