#pragma once

#include <Coco/Core/API.h>

#include <memory>

namespace Coco
{
	/// @brief Class that manages the lifetime of an object and can be shared
	/// @tparam ValueType 
	template<typename ValueType>
	using Ref = std::shared_ptr<ValueType>;
	
	/// @brief Creates a managed object that can be shared
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object that can be shared
	template<typename ValueType, typename ... Args>
	Ref<ValueType> COCOAPI CreateRef(Args&& ... args)
	{
		return std::make_shared<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief A weak reference to a reference
	/// @tparam ValueType 
	template<typename ValueType>
	using WeakRef = std::weak_ptr<ValueType>;
	
	/// @brief Class that manages the lifetime of an object
	/// @tparam ValueType 
	/// @tparam U 
	template<typename ValueType, typename ValueDeleter = std::default_delete<ValueType>>
	using Managed = std::unique_ptr<ValueType, ValueDeleter>;

	/// @brief Creates a managed object
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object
	template<typename ValueType, typename ... Args>
	Managed<ValueType> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief Creates a managed object with a custom deleter
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @tparam U 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object
	template<typename ValueType, typename U, typename ... Args>
	Managed<ValueType, U> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<ValueType, U>(std::forward<Args>(args)...);
	}

	/// @brief Downcasts a Ref without checking if it actually is the given type
	/// @tparam ToType 
	/// @tparam FromType 
	/// @param from The starting reference
	/// @return A down-casted reference
	template<typename ToType, typename FromType>
	Ref<ToType> RefCast(const Ref<FromType>& from)
	{
		return std::static_pointer_cast<ToType>(from);
	}

	/// @brief Downcasts a Ref and does a check to see if it actually is the given type
	/// @tparam ToType 
	/// @tparam FromType 
	/// @param from The starting reference
	/// @return A down-casted reference, or nullptr if the original reference is not the downcasted type
	template<typename ToType, typename FromType>
	Ref<ToType> SafeRefCast(const Ref<FromType>& from)
	{
		return std::dynamic_pointer_cast<ToType>(from);
	}

	/// @brief Downcasts a Managed without checking if it actually is the given type
	/// @tparam ToType 
	/// @tparam FromType 
	/// @param from The starting reference
	/// @return A down-casted reference
	template<typename ToType, typename FromType>
	Managed<ToType> ManagedCast(const Managed<FromType>& from)
	{
		return std::static_pointer_cast<ToType>(from);
	}
}